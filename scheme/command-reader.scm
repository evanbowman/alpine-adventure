;;;
;;; On-screen repl
;;;


(define command-display-widjet-pool '())


(define (command-get-display-widjet)
  (if (not (null? command-display-widjet-pool))
      (let ((ret (car command-display-widjet-pool)))
        (set! command-display-widjet-pool (cdr command-display-widjet-pool))
        ret)
      (begin
        (Game_log "command created display object")
        (Game_makeWidjet))))


(define (command-recycle-display-widjet widjet)
  (set! command-display-widjet-pool (cons widjet command-display-widjet-pool)))


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
(define command-text-bar
  ((lambda ()
     (let ((bar (Game_makeWidjet))
           (window-width (vector-ref window-info 2))
           (window-height (vector-ref window-info 2)))
       (Object_setFace bar spr-pixel)
       (Object_setVisible bar #f)
       (Object_setFaceColor bar #(128 128 128 128))
       (Object_setFaceScale bar window-width 32.0)))))


(define (command-push-char! unicode-char window-info)
  (set! command-input (cons unicode-char command-input))
  (let ((glyph (command-get-display-widjet)))
    ;; TODO: set properties for glyph
    (set! command-display-list (cons glyph command-display-list))))


(define (command-pop-char!)
  (if (not (null? command-input))
      (begin
        (set! command-input (cdr command-input))
        (let ((recycle-widjet (car command-display-list)))
          (set! command-display-list (cdr command-display-list))
          (command-recycle-display-widjet recycle-widjet)
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
  (Object_setVisible command-text-bar #t)
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
            (Object_setVisible command-text-bar #f)
            (Game_log "closed command reader")
            (Game_sleep 1000000))
          (loop)))))
