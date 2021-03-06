(define-module mac-address
  (use gauche.sequence)
  (use gauche.uvector)
  (export mac-address-cmp
	  mac-address-hash
	  string->mac-address))

(select-module mac-address)

(define (mac-address-cmp a b)
  (and (= (u8vector-length a) (u8vector-length b))
       (every = (u8vector->list a) (u8vector->list b))))

(define (mac-address-hash mac max)
  (modulo (apply + (u8vector->list mac)) max))

(define (string->mac-address str)
  (list->u8vector (map (cut string->number <> 16)
		       (string-split str #[-:]))))
