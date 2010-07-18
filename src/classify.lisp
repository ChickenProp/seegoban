;; My intuition is that Manhattan distance will give better results than
;; Euclidean, because it increases faster when all the components change. But I
;; can test that later.
(defun distance (p1 p2)
  (apply #'+ (map 'list (lambda (x y) (abs (- x y)))
		  p1 p2)))

(defun cartesian-product (&rest sets)
  (if (null sets)
      (list nil)
      (loop for x in (first sets)
	 appending (loop for y in (apply #'cartesian-product (rest sets))
		      collecting (cons x y)))))

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

(setq *print-circle* t)

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
  (let ((c-points (mapcar #'cluster points)))
    (iterate (- (length points) n)
	     #'reduce-sorted-distance-pairs
	     (c-point-distances c-points))
    (mapcar (lambda (clust)
	      (mapcar #'c-point-point (flatten clust)))
	    (uniq (mapcar #'c-point-cluster c-points)))))

;; Some points to try clustering
(defvar *points*
  '((45 11)
    (44 16)
    (40 17)
    (42 30)
    (43 17)
    (45 21)
    (90 32)
    (105 12)
    (110 14)
    (152 13)
    (160 18)
    (145 23)
    (110 27)
    (170 26)
    (250 6)
    (250 7)
    (245 10)
    (235 20)
    (230 15) 
    (231 16)
    (246 13)))

(princ (time (clusterize 3 *points*)))
