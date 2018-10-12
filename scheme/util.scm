;;;
;;; Useful misc. stuff not related specifically to anything else
;;;


(define (->string object)
  (define port (open-output-string))
  (write object port)
  (get-output-string port))


(define (log-info str)
  (Game_log Log_info str))


(define (log-warning str)
  (Game_log Log_warning str))
