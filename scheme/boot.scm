;;
;; Startup stuff
;;


(Game_log "starting up...")
(Game_setVSyncEnabled #t)
(Game_setFramerateLimit 60)
(Game_log "configuration complete")
(define spr-txtr-0001 (Game_createTexture "texture/Sprite-0001.png"))
(define spr-txtr-0002 (Game_createTexture "texture/Sprite-0002.png"))
(define spr-glyphs (Game_createTexture "texture/ubuntu-mono-18.png"))
(define spr-pixel (Game_createTexture "texture/pixel.png"))
(define spr-vignette (Game_createTexture "texture/vignette.png"))
(Game_log "finished loading textures")

(define window-info (Game_describeWindow))


(define vignette-overlay
  (let ((obj (Game_makeWidjet)))
    (Object_setFace obj spr-vignette)
    (Object_setFaceScale obj
                         (/ (vector-ref window-info 2) 450.0)
                         (/ (vector-ref window-info 3) 450.0))
    (Object_setFaceColor obj #(255 255 255 181))
    (Object_setZOrder obj 1)
    obj))


(define (player)
  (let ((obj (Game_makeObject)))
    (Object_setFace obj spr-txtr-0001)
    (Object_setShadow obj spr-txtr-0002)
    obj))
