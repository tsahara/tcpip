(use gauche.net)

;; IPv6 address is (bytes scope-id)

(define (string->ipv6-address str)
  (let* ((buf (make-u8vector 16))
	 (af (inet-string->address! str buf)))
    (if (not (= af AF_INET6))
	(error "~a is not ipv6 address" str))
    (list buf 0)))

(define (ipv6-address->u8vector v6addr)
  (car v6addr))
