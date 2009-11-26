vs_1_1
dcl_position v0
dcl_color v1
dcl_texcoord v2
dcl_normal v3

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; c0 - c3 is view matrix           ;;
;; c4 - c7 is 1st bone matrix       ;;
;; c8 - c11 is 2nd bone matrix      ;;
;;                                  ;;
;; c100 is constant 0.0f            ;;
;; c111 is constant 1.0f            ;;
;;                                  ;;
; !r1  is transformed vertex        ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

def c100, 0.0, 0.0, 0.0, 0.0
def c111, 1.0, 1.0, 1.0, 1.0

;;;;;;;;;;;;;;;;;;;;;; Skinning ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; - - - - - - - - - -  position  - - - - - - - - - - - - - -;
m4x4 r1, v0, c4
mul r1.xyz, r1.xyz, v2.x            ; first bone
m4x4 r0, v0, c8
mad r0.xyz, r0.xyz, v2.y, r1.xyz    ; second bone
m4x4 r1, r0, c27                    ; position and rotation
m4x4 r2, r1, c31  ; projection to plane

;;;;;;;;;;;;;;;;;;;;;;;; Results ;;;;;;;;;;;;;;;;;;;;;;;;;;;;
m4x4 oPos, r2, c0
mov oD0, c100     ; black color
