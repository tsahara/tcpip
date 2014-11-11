;;;
;;; pcap
;;;

(define-module pcap
  (export test-pcap ;; dummy
	  <pcap> make-pcap
          )
  )
(select-module pcap)

;; Loads extension
(dynamic-load "pcap")

;;
;; Put your Scheme definitions here
;;



