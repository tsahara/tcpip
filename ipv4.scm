(define-module ipv4
  (use binary.io)
  (use gauche.net)
  (use gauche.uvector)
  (export string->ipv4-address))

(select-module ipv4)

;;
;; ipv4-address := (u8vector :length 4)
;;

(define (string->ipv4-address str)
  (receive (num af)
      (inet-string->address str)
    (let1 v (make-u8vector 4)
      (put-u32be! v 0 num)
      v)))
