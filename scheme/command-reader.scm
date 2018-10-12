;;;
;;; On-screen repl
;;;


(define command-display-obj-pool '())


(define (command-get-display-obj)
  (if (not (null? command-display-obj-pool))
      (let ((ret (car command-display-obj-pool)))
        (set! command-display-obj-pool (cdr command-display-obj-pool))
        ret)
      (begin
        (Game_log "command created display object")
        (Game_makeObject))))


(define (command-recycle-display-obj obj)
  (set! command-display-obj-pool (cons obj command-display-obj-pool)))


(import (chibi ast))

(define (command-execute str)
  (call-with-current-continuation
   (lambda (return)
     (with-exception-handler
         (lambda (err)
           (return (exception-message err)))
       (lambda ()
         (return (->string (eval (read (open-input-string str))))))))))


(define command-input '())
(define command-display-list '())


(define (command-push-char! unicode-char window-info)
  (set! command-input (cons unicode-char command-input))
  (set! command-display-list (cons (command-get-display-obj)
                                   command-display-list)))


(define (command-pop-char!)
  (if (not (null? command-input))
      (begin
        (set! command-input (cdr command-input))
        (let ((recycle-obj (car command-display-list)))
          (set! command-display-list (cdr command-display-list))
          (command-recycle-display-obj recycle-obj)
          #t))
      #f))


(define (command-clear!)
  (if (not (command-pop-char!))
      '()
      (command-clear!)))


(define command-modifier-key-states
  (list (cons Key_backspace (cons #t #f))
        (cons Key_return    (cons #t #f))
        (cons Key_esc       (cons #t #f))))


(define (command-modifier-pressed? key)
  (let ((found (assoc key command-modifier-key-states)))
    (cond
     ((not found) (error "invalid modifier key"))
     (else
      (and (car (cdr found)) (cdr (cdr found)))))))


(define (command-update-modifier-states!)
  (let update ((current command-modifier-key-states))
    (cond
     ((null? current) '())
     (else
      (let ((state (cdr (car current)))
            (key (Game_keyPressed (car (car current)))))
        (set-cdr! state (not (equal? key (car state))))
        (set-car! state key))
      (update (cdr current))))))


(define (command-get-input receiver)
  (let ((unicode-char (Game_pollTextChannel)))
    (if (not unicode-char)
        '()
        (begin
          (receiver unicode-char)))))


(define (command-drain-text)
  (if (not (Game_pollTextChannel))
      '()
      (command-drain-text)))


(define (command-reader)
  (Game_log "opened command reader")
  (Game_setTextChannelActive #t)
  (let ((window-info (Game_describeWindow)))
    (let loop ()
      (Game_sleep 10000)
      (command-update-modifier-states!)
      (cond
       ((command-modifier-pressed? Key_backspace)
        (command-pop-char!))
       ((command-modifier-pressed? Key_return)
        (Game_log (command-execute (int-list->string command-input)))
        (command-clear!)))
      (command-get-input
       (lambda (unicode-char)
         (if (and (not (equal? unicode-char 8))
                  (not (equal? unicode-char 10)))
             (command-push-char! unicode-char window-info)
             (int-list->string command-input))))
      (if (command-modifier-pressed? Key_esc)
          (begin
            (command-clear!)
            (Game_setTextChannelActive #f)
            (command-drain-text)
            (Game_log "closed command reader")
            (Game_sleep 1000000))
          (loop)))))
