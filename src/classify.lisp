#! /usr/bin/sbcl --script

(defstruct point
  name
  coords
  format)

(defun list-to-point (lst)
  (make-point :name (car lst) :coords (cdr lst)
	      :format (cond ((= (list-length lst) 3)
			     :bs)
			    ((= (list-length lst) 4)
			     :rgb)
			    (t nil))))

(defun rgb-to-bs (rgb)
  "Converts (r g b) to (brightness saturation)."
  (list (+ (* 0.30 (first rgb))
	   (* 0.59 (second rgb))
	   (* 0.11 (third rgb)))
	(- (apply #'max rgb)
	   (apply #'min rgb))))

(defun point-to-bs (p)
  (make-point :name (point-name p) :format :bs
	      :coords (case (point-format p)
			(:rgb (rgb-to-bs (point-coords p)))
			(:bs (point-coords p)))))

(defun l1-distance (lst1 lst2)
  (assert (= (length lst1) (length lst2)))
  (apply #'+ (map 'list
		  (lambda (x y) (abs (- x y)))
		  lst1 lst2)))

(defun distance-rgb (p1 p2)
  (l1-distance (point-coords p1) (point-coords p2)))

(defun distance-bs (p1 p2)
  (l1-distance (point-coords (point-to-bs p1))
	       (point-coords (point-to-bs p2))))

(defvar *distance-function*
  #'distance-bs
  "The function used to calculate the distance between two points.")

(defun distance (p1 p2)
  "Calls *distance-function* on p1 and p2."
  (funcall *distance-function* p1 p2))

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
  (list-to-point (cons :mean
		       (apply #'map 'list #'mean
			      (mapcar #'point-coords points)))))

(defun cluster-union (&rest clusters)
  (let ((points (apply #'union (map 'list #'cluster-points clusters))))
    (make-cluster :points points
		  :centre (point-mean points))))

(defun cluster-distance (c1 c2)
  (distance (cluster-centre c1) (cluster-centre c2)))

(defun reduce-clusters (arg)
  (destructuring-bind (clusters pairs) arg
    (let ((best-1 (cadar pairs))
	  (best-2 (caddar pairs)))
      (if (or (cluster-obsolete best-1) (cluster-obsolete best-2))
	  (reduce-clusters (list clusters (cdr pairs)))

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

(defun clusterize-slow (n points)
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
				     #'reduce-clusters
				     (list clusters pairs)))))))


;; The k-means algorithm is fast, but depends on initial conditions.

(defun closest-point (target points)
  (apply #'arg-min (lambda (x) (distance target x)) points))

(defun clusterize-points-by-means (points means
				   &optional (acc (make-list (length means))))
  (if (null points)
      acc
      (let ((index (position (closest-point (car points) means) means
			     :test #'equal)))
	(setf (nth index acc) (cons (car points)
				    (nth index acc)))
	(clusterize-points-by-means (cdr points) means acc))))

(defun kmeans-iterate-means (means points)
  (mapcar #'point-mean (clusterize-points-by-means points means)))

(defun clusterize-kmeans (points initial)
  ; 10 is a fairly arbitrary number that deserves investigation.
  (let ((means (iterate 10
			(lambda (means)
			  (kmeans-iterate-means means points))
			initial)))
    (append (clusterize-points-by-means points means)
	    (mapcar #'list means))))

;; k-means++ is a way of finding initial means for k-means.
(defun clusterize-kmeans++ (n points)
  n
  (clusterize-kmeans points
		     ; for now this is arbitrary
		     (list (list-to-point '(:a 0 0 0))
			   (list-to-point '(:b 255 255 255))
			   (list-to-point '(:c 220 200 140)))))

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
	     do (destructuring-bind (brt sat) (point-coords (point-to-bs point))
		  (format t control-string brt sat)))
	  (format t "~%"))))

(defun print-clusters (output-type clusters)
  (case (intern (string-upcase output-type) 'keyword)
    (:graph (print-clusters-graph clusters))
    (:lisp (print clusters))
    (t (format t "Unknown output format '~S'.~%" output-type))))

;; Some points to try clustering, for testing purposes. A subset from board-1.
(defvar *test-points*
  '((:1 226 202 143)
    (:2 223 203 146)
    (:3 224 201 140)
    (:4 228 205 142)
    (:5 218 197 147)
    (:6 227 201 141)
    (:7 228 198 137)
    (:8 230 199 139)
    (:9 228 198 138)
    (:10 225 193 133)

    (:11 211 200 168)
    (:12 234 230 217)
    (:13 199 197 182)
    (:14 200 190 162)
    (:15 237 237 226)
    (:16 245 244 239)
    (:17 203 191 163)
    (:18 227 222 208)
    (:19 218 210 196)
    (:20 242 240 234)

    (:21 118 120 112)
    (:22 99 96 93)
    (:23 149 148 140)
    (:24 93 94 85)
    (:25 76 73 69)
    (:26 48 49 38)
    (:27 42 39 36)
    (:28 23 25 16)
    (:29 83 85 78)
    (:30 76 78 71)))

(let ((output-type :lisp)
      (algorithm #'clusterize)
      (points-type :real))
  (dolist (key (mapcar (lambda (str)
			 (intern (string-upcase str) 'keyword))
		       *posix-argv*))
    (case key
      (:graph (setf output-type :graph))
      (:slow (setf algorithm #'clusterize-slow))
      (:kmeans (setf algorithm #'clusterize-kmeans++))
      (:test (setf points-type :test))
      (:rgb (setf *distance-function* #'distance-rgb))))

  (let ((points (if (eq points-type :real)
		    (read)
		    *test-points*)))
    (print-clusters output-type
		    (funcall algorithm 3
			     (mapcar #'list-to-point points))))
  
  (fresh-line))
