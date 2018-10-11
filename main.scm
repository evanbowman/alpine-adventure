;;;
;;; Entry point for the program
;;;

(load "command-reader.scm")


(Game_log "starting up...")
(Game_setVSyncEnabled #t)
(Game_setFramerateLimit 60)
(Game_log "configuration complete")
(define spr-txtr-0001 (Game_createTexture "Sprite-0001.png"))
(define spr-txtr-0002 (Game_createTexture "Sprite-0002.png"))
(Game_log "finished loading textures")


(define (player)
  (let ((obj (Game_makeObject)))
    (Object_setFace obj spr-txtr-0001)
    (Object_setShadow obj spr-txtr-0002)
    obj))


(define test-obj (player))

(Game_setCameraTarget test-obj)


(define (update-game)
  (cond
   ((Game_keyPressed Key_esc)
    (command-reader)))
  (Game_update))


(let main-loop ()
  (cond
   ((not (Game_isRunning))
    '())
   (else
    (update-game)
    (main-loop))))
