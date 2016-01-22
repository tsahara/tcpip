(define-module ethernet
  (export <ethernet>
	  make-ethernet
	  )
  )
(select-module ethernet)

(define-class <ethernet> (<network-interface>)
  ((bpf                :init-value -1)
   (physical-interface)
   ))

(define (make-ethernet ip-stack phy)
  (let1 ethernet (make <ethernet>)
    (slot-set! ethernet 'ip-stack ip-stack)
    (slot-set! ethernet 'name "eth0")

    (slot-set! ethernet 'bpf (bpf-open))
    (slot-set! ethernet 'physical-interfacee phy)

    (selector-add! (ref ip-stack 'selector)
                   (ref ethernet 'bpf)
                   ethernet-receive-packet
                   '(r))
    ))

(define (ethernet-receive-packet bpf flag)
  (print "ethernet received")
  )
