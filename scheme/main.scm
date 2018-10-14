;;;
;;; Entry point for the program
;;;

(load "scheme/util.scm")
(load "scheme/boot.scm")
(load "scheme/command-reader.scm")


(define test-obj (player))

(Game_setCameraTarget test-obj)


(define (update-game)
  (if (Game_keyPressed Key_esc)
      (command-reader))
  (Game_update))


(let main-loop ()
  (cond
   ((not (Game_isRunning))
    (Game_log "main loop exited normally"))
   (else
    (update-game)
    (main-loop))))
