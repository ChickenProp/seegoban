#! /usr/bin/sbcl --script

(defstruct point
  name
  coords)

(defun list-to-point (lst)
  (make-point :name (car lst) :coords (cdr lst)))

;; My intuition is that Manhattan distance will give better results than
;; Euclidean, because it increases faster when all the components change. But I
;; can test that later.

;; p1 and p2 are taken to be lists: (name coords...).
(defun distance (p1 p2)
  (apply #'+ (map 'list
		  (lambda (x y) (abs (- x y)))
		  (point-coords p1)
		  (point-coords p2))))

(defun combinations (seq n)
  "Returns the possible order-independent combinations of n distinct elements
from seq."
  (cond ((= n 0)
	 (list nil))
	((> n (length seq))
	 nil)
	(t
	 (let ((lst (coerce seq 'list)))
	   (append (mapcar (lambda (s)
			     (cons (car lst) s))
			   (combinations (cdr lst) (- n 1)))
		   (combinations (cdr lst) n))))))

(defun iterate (n fn start)
  "(fn (fn (fn ... (fn start)...))), where fn is called n times."
  (if (= n 0)
      start
      (iterate (- n 1) fn (funcall fn start))))

(defun uniq (lst &optional (test #'eql))
  "Returns lst with duplicate elements removed."
  (if (null lst)
      nil
      (let ((uniq-cdr (uniq (cdr lst) test)))
	(if (member (car lst) uniq-cdr :test test)
	    uniq-cdr
	    (cons (car lst) uniq-cdr)))))

(defun flatten (lst)
  (cond ((atom lst)
	 lst)
	((atom (car lst))
	 (cons (car lst) (flatten (cdr lst))))
	(t
	 (append (flatten (car lst))
		 (flatten (cdr lst))))))

(defun merge-sorted (lst1 lst2 test &key (key #'identity))
  (let ((keep lst2))
    (loop
       until (or (null lst1) (null lst2))
       do (if (funcall test (funcall key (car lst1)) (funcall key (car lst2)))
	      (let ((oldcar (car lst2)))
		(setf (car lst2) (car lst1))
		(setf (cdr lst2) (cons oldcar (cdr lst2)))
		(setf lst1 (cdr lst1)
		      lst2 (cdr lst2)))
	      (setf lst2 (cdr lst2))))
    keep))

(defun dotree* (tree fn)
  (cond ((null tree)
	 nil)
	((consp tree)
	 (dotree* (car tree) fn)
	 (dotree* (cdr tree) fn))
	(t
	 (funcall fn tree))))

(defmacro dotree ((var tree &optional ret) &body body)
  `(progn
     (dotree* ,tree (lambda (,var) ,@body))
     ,ret))

(defun arg-min (fn val &rest more-vals)
  "Returns the value minimizing (fn val)"
  (cond ((null more-vals)
	 val)
	((> (funcall fn val) (funcall fn (car more-vals)))
	 (apply #'arg-min fn more-vals))
	(t
	 (apply #'arg-min fn (cons val (cdr more-vals))))))

(defun apply-fn (fn)
  (lambda (x) (apply fn x)))

;; This is a fast algorithm that relies on cluster-distance being min distance
;; between points in each cluster. Below is a slower one that can use an
;; arbitrary cluster-distance.
;; (Fast is O(n^2 log n), with the complexity bottleneck being the sort.)

(defstruct c-point ; abbreviation for clustered-point
  point
  cluster)

(defun cluster (point)
  (let ((cp (make-c-point :point point :cluster nil)))
    (setf (c-point-cluster cp) (list cp))
    cp))

(defun c-point-distances (c-points)
  (sort (map 'list
	     (lambda (x)
	       (cons (distance (c-point-point (car x))
			       (c-point-point (cadr x)))
		     x))
	     (combinations c-points 2))
	#'<
	:key #'car))

(defun reduce-sorted-distance-pairs (sdps)
  (let* ((best (car sdps))
	 (c-point-1 (second best))
	 (clust-1 (c-point-cluster c-point-1))
	 (c-point-2 (third best))
	 (clust-2 (c-point-cluster c-point-2)))
    
    (if (eq (c-point-cluster c-point-1)
	    (c-point-cluster c-point-2))
	(reduce-sorted-distance-pairs (cdr sdps))
	(progn
	  (dotree (c-point clust-2)
	    (setf (c-point-cluster c-point) clust-1))
	  (setf (cdr clust-1) (cons (car clust-1) (cdr clust-1)))
	  (setf (car clust-1) clust-2)
	  (cdr sdps)))))

(defun clusterize (n points)
  (let* ((c-points (mapcar #'cluster points))
	 (distances (c-point-distances c-points)))
    (iterate (- (length points) n)
	     #'reduce-sorted-distance-pairs
	     distances)
    (mapcar (lambda (clust)
	      (mapcar #'c-point-point (flatten clust)))
	    (uniq (mapcar #'c-point-cluster c-points)))))


;; This is the slow algorithm. I don't even want to imagine its complexity.

(defstruct cluster
  points
  centre
  obsolete)

(defun cluster-wrap-point (pt)
  (make-cluster :points (list pt) :centre pt))

(defun mean (&rest vals)
  (/ (apply #'+ vals) (length vals)))

(defun point-mean (points)
  (make-point :name nil
	      :coords (apply #'map 'list #'mean
			     (mapcar #'point-coords points))))

(defun cluster-union (&rest clusters)
  (let ((points (apply #'union (map 'list #'cluster-points clusters))))
    (make-cluster :points points
		  :centre (point-mean points))))

(defun cluster-distance (c1 c2)
  (distance (cluster-centre c1) (cluster-centre c2)))

(defun nearest-clusters (clusters)
  (apply #'arg-min
	 (apply-fn #'cluster-distance)
	 (combinations clusters 2)))

(defun reduce-clusters (clusters)
  (let ((nearest (nearest-clusters clusters)))
    (append (remove-if (lambda (cluster) (find cluster nearest :test #'equal))
		       clusters)
	    (list (apply #'cluster-union nearest)))))

(defun reduce-to-n-clusters (n clusters)
  (if (<= (length clusters) n)
      clusters
      (reduce-to-n-clusters n (reduce-clusters clusters))))

(defun clusterize-slow (n points)
  (reduce-to-n-clusters n (map 'list #'cluster-wrap-point
			       points)))

(defun reduce-clusters-fast (arg)
  (destructuring-bind (clusters pairs) arg
    (let ((best-1 (cadar pairs))
	  (best-2 (caddar pairs)))
      (if (or (cluster-obsolete best-1) (cluster-obsolete best-2))
	  (reduce-clusters-fast (list clusters (cdr pairs)))

	  (let* ((best-union (cluster-union best-1 best-2))
		 (pairs-with-best
		  (sort (loop for clust in clusters
			   nconcing (if (cluster-obsolete clust)
					nil
					(list (list (cluster-distance best-union
								      clust)
						    best-union
						    clust))))
			#'<
			:key #'car)))

	    (setf (cluster-obsolete best-1) t
		  (cluster-obsolete best-2) t)
	    (list (cons best-union clusters)
		  (merge-sorted pairs-with-best pairs #'< :key #'car)))))))

(defun clusterize-slow-fast (n points)
  (let* ((clusters (map 'list #'cluster-wrap-point points))
	 (pairs (sort (map 'list
			   (lambda (pair)
			     (cons (cluster-distance (car pair)
						     (cadr pair))
				   pair))
			   (combinations clusters 2))
		      #'<
		      :key #'car)))
    (mapcar #'cluster-points
	    (remove-if #'cluster-obsolete
		       (car (iterate (- (length points) n)
				     #'reduce-clusters-fast
				     (list clusters pairs)))))))


;; If there's only one point in a set, xgraph doesn't draw it. So we place two
;; points in the same position instead, so they at least show up with -m,
;; -M, -p or -P.
;; (~@* means "go back to the first argument".)
(defun print-clusters-graph (clusters)
  (loop for clust in clusters
     do (let ((control-string (if (= (length clust) 1)
				  "~F ~F~%~@*~F ~F~%"
				  "~F ~F~%")))
	  (loop for point in clust
	     ;; If there are two coordinates, brightness and saturation.
	     ;; Otherwise, rgb.
	     do (let ((coords (point-coords point)))
		  (if (= (list-length coords) 2)
		      (format t control-string
			      (first coords) (second coords))
		      (format t control-string
			      (+ (* 0.30 (first coords))
				 (* 0.59 (second coords))
				 (* 0.11 (third coords)))
			      (- (apply #'max coords)
				 (apply #'min coords))))))
	  (format t "~%"))))

(defun print-clusters (output-type clusters)
  (case (intern (string-upcase output-type) 'keyword)
    (:graph (print-clusters-graph clusters))
    (:lisp (print clusters))
    (t (format t "Unknown output format '~S'.~%" output-type))))

;; Some points to try clustering, for testing purposes
(defvar *test-points*
  '((:1 45 11)
    (:2 44 16)
    (:3 40 17)
    (:4 42 30)
    (:5 43 17)
    (:6 45 21)
    (:7 90 32)
    (:8 105 12)
    (:9 110 14)
    (:10 152 13)
    (:11 160 18)
    (:12 145 23)
    (:13 110 27)
    (:14 170 26)
    (:15 250 6)
    (:16 250 7)
    (:17 245 10)
    (:18 235 20)
    (:19 230 15) 
    (:20 231 16)
    (:21 246 13)))

(let ((output-type :lisp)
      (algorithm #'clusterize)
      (points-type :real))
  (dolist (key (mapcar (lambda (str)
			 (intern (string-upcase str) 'keyword))
		       *posix-argv*))
    (case key
      (:graph (setf output-type :graph))
      (:slow (setf algorithm #'clusterize-slow-fast))
      (:test (setf points-type :test))))

  (let ((points (if (eq points-type :real)
		    (read)
		    *test-points*)))
    (print-clusters output-type
		    (funcall algorithm 3
			     (mapcar #'list-to-point points))))
  
  (fresh-line))
