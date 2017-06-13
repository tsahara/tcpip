(define-module surt.network-interface
  (export <network-interface>
	  network-interface-name
	  )
  )
(select-module surt.network-interface)

(define-class <network-interface> ()
  ((ip-stack)
   (name     :init-value "")
   ))

(define-method network-interface-name ((netif <network-interface>))
  (slot-ref netif 'name))
