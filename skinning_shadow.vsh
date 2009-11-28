vs_1_1
dcl_position v0
dcl_texcoord v2

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; c0 - c3 is view matrix           ;;
;; c4 - c7 is 1st bone matrix       ;;
;; c8 - c11 is 2nd bone matrix      ;;
;; c17 is point light position      ;;
;; c31-c34 is shadow proj. matrix   ;;
;; c35 are shadow attenuat. consts  ;;
;;                                  ;;
;; c100 is constant 0.0f            ;;
;; c111 is constant 1.0f            ;;
;;                                  ;;
; !r3  is transformed vertex        ;;
; !r10 is result color              ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

def c100, 0.0, 0.0, 0.0, 0.0
def c111, 1.0, 1.0, 1.0, 1.0
def c105, 0.5, 0.5, 0.5, 0.5

;;;;;;;;;;;;;;;;;;;;;; Skinning ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; - - - - - - - - - -  position  - - - - - - - - - - - - - -;
m4x4 r1, v0, c4
mul r1.xyz, r1.xyz, v2.x            ; first bone
m4x4 r0, v0, c8
mad r0.xyz, r0.xyz, v2.y, r1.xyz    ; second bone
m4x4 r1, r0, c27                    ; position and rotation
m4x4 r3, r1, c31  ; projection to plane
rcp r4, r3
mul r3, r3, r4.w

; - - - - - - - - - - - color - - - - - - - - - - - - - - - ;
; calculating direction vector
add r11, c17, -r3       ; r11 = position(point) - position(vertex)
dp3 r2, r11, r11        ; r2 = distance**2
rsq r7, r2              ; r7 = 1/distance
; calculating attenuation
dst r2, r2, r7          ; r2 = (1, d, d**2, 1/d)
dp3 r0, r2, c35         ; r0 = (a + b*d + c*d**2)
rcp r0, r0              ; r0 = attenuation coef
mov r10, c100           ; r10 = black
mul r10.a, c111.a, r0.a ; r10.alpha = 1 * attenuation

;;;;;;;;;;;;;;;;;;;;;;;; Results ;;;;;;;;;;;;;;;;;;;;;;;;;;;;
m4x4 oPos, r3, c0
mov oD0, r10
