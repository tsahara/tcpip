(define-module surt.ethernet
  (use bpf)
  (use gauche.selector)
  (use surt.network-interface)
  (export <ethernet>
	  make-ethernet
	  ethernet-send
	  )
  )
(select-module surt.ethernet)

(define-class <ethernet> (<network-interface>)
  ((bpf                :init-value -1)
   (physical-interface)
   ))

(define (make-ethernet ip-stack phy)
  (let1 ethernet (make <ethernet>)
    (slot-set! ethernet 'ip-stack ip-stack)
    (slot-set! ethernet 'name "eth0")

    (slot-set! ethernet 'bpf (bpf-open phy))
    (slot-set! ethernet 'physical-interface phy)

    (selector-add! (ref ip-stack 'selector)
                   (ref ethernet 'bpf)
                   ethernet-receive-packet
                   '(r))
    ethernet))

(define (ethernet-receive-packet bpf flag)
  (print "ethernet received")
  )

(define (ethernet-send packet)
  (print "ethernet received")
  )
