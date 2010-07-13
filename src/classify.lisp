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

(defun cluster-distance (c1 c2)
  (apply #'min (map 'list
		    (lambda (x) (apply #'distance x))
		    (cartesian-product c1 c2))))

(defun nearest-clusters (clusters)
  (apply #'arg-min
	 (apply-fn #'cluster-distance)
	 (remove-if (apply-fn #'equal)
		    (cartesian-product clusters clusters))))

(defun reduce-clusters (clusters)
  (let ((nearest (nearest-clusters clusters)))
    (append (remove-if (lambda (cluster) (find cluster nearest :test #'equal))
		       clusters)
	    (list (apply #'union nearest)))))

(defun reduce-to-n-clusters (n clusters)
  (if (<= (length clusters) n)
      clusters
      (reduce-to-n-clusters n (reduce-clusters clusters))))

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

(defvar *clusters* (mapcar #'list *points*))

(time (princ (reduce-to-n-clusters 3 *clusters*)))
