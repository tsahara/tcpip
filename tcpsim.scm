(use data.heap)

(define-class <tcp> ()
  ())

(define (tcp-open-to peer)
  (timer-event-add 1 (lambda () (print "a event"))))

(define-class <tcp-segment> ()
  ())


(define *future-events* (make-binary-heap :key car))
(define *virtual-clock* 0)

(define (timer-event-add sec thunk)
  (binary-heap-push! *future-events* (cons (+ *virtual-clock* sec) thunk)))

(define (event-loop)
  (unless (binary-heap-empty? *future-events*)
    (let1 ev (binary-heap-pop-min! *future-events*)
      (inc! *virtual-clock* (car ev))
      (format #t "~4@a.~3,0a : " (floor *virtual-clock*)
	      (- *virtual-clock* (floor *virtual-clock*)))
      ((cdr ev))
      (event-loop))))

(let ((a (make <tcp>))
      (b (make <tcp>)))
  (tcp-open-to b)
  (event-loop))
