(use gauche.test)
(use ipv4)

(test-start "mac-address")
(load "mac-address")
(import mac-address)

(test-module 'mac-address)

(test-section "MAC Address")

(let ((mac0 (string->mac-address "02:00:00:00:00:00"))
      (mac1 (string->mac-address "02:01:01:01:01:01")))
  (test* "compare equal" #t (mac-address-cmp mac0 mac0))
  (test* "compare differ" #f (mac-address-cmp mac0 mac1))

  )

(test-end)
