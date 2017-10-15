(define-module macaddr
  (use gauche.uvector)
  (export string->macaddr))

(select-module macaddr)

(define (string->macaddr str)
  (list->u8vector (map (cut string->number <> 16)
		       (string-split str #[-:]))))
