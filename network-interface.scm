(define-module surt.network-interface
  (export <network-interface>
	  )
  )
(select-module surt.network-interface)

(define-class <network-interface> ()
  ((ip-stack)
   (name     :init-value "")
   ))
