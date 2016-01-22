(define-module network-interface
  (export <network-interface>
	  )
  )
(select-module network-interface)

(define-class <network-interface> ()
  ((ip-stack)
   (name     :init-value "")
   ))
