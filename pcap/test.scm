;;;
;;; Test pcap
;;;

(use gauche.test)

(test-start "pcap")
(use pcap)
(test-module 'pcap)

;; The following is a dummy test code.
;; Replace it for your tests.
(test* "test-pcap" "pcap is working"
       (test-pcap))

;; If you don't want `gosh' to exit with nonzero status even if
;; the test fails, pass #f to :exit-on-failure.
(test-end :exit-on-failure #t)




