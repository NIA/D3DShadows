vs_1_1
dcl_position v0

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; c0 - c3 is view matrix           ;;
;; c4  is FINAL_RADIUS              ;;
;; c5  is t                         ;;
;; c17 is point light position      ;;
;; c27 - c30 is pos.*rot. matrix    ;;
;; c31-c34 is shadow proj. matrix   ;;
;; c35 are shadow attenuat. consts  ;;
;;                                  ;;
;; c100 is constant 0.0f            ;;
;; c111 is constant 1.0f            ;;
;;                                  ;;
; !r1  is transformed vertex        ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

def c100, 0.0, 0.0, 0.0, 0.0
def c111, 1.0, 1.0, 1.0, 1.0 ;constant one

;;;;;;;;;;;;;;;;;;;;;; Morphing ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; - - - - - - - - - -  position  - - - - - - - - - - - - - -;
dp3 r0, v0, v0
rsq r0, r0              ; r0 = 1/|r|

mov r2, c111            ; r2 = 1.0f
mad r1, r0, c4, -r2    ; r1 = ((FINAL_RADIUS/r0) - 1)
mad r1, r1, c5, r2     ; r1 = 1+t*((FINAL_RADIUS/r0) - 1)

mul r2.xyz, v0.xyz, r1.x

m4x4 r1, r2, c27        ; position and rotation
m4x4 r3, r1, c31        ; projection to plane

;;;;;;;;;;;;;;;; Results: coordinates ;;;;;;;;;;;;;;;;;;;;;;;
m4x4 oPos, r3, c0

; - - - - - - - - dividing to .w - - - - - - - - - - - - ;
rcp r7, r3
mul r3, r3, r7.w

; - - - - - - - - - - - color - - - - - - - - - - - - - - - ;
; calculating direction vector
add r11, c17, -r3       ; r11 = position(point) - position(vertex) = AC
dp3 r4, r11, r11        ; r4 = distance**2 = |AC|^2
rsq r7, r4              ; r7 = 1/distance
; calculating attenuation
dst r2, r4, r7          ; r2 = (1, d, d**2, 1/d)
dp3 r0, r2, c35         ; r0 = (a + b*d + c*d**2)
rcp r0, r0              ; r0 = attenuation coef
mov r10, c100           ; r10 = black
mul r10.a, c111.a, r0.a ; r10.alpha = 1 * attenuation
; - - - - - - - - - - checking - - - - - - - - - - - - - - - ;
; A - on the plane, B - vertex, C - light source
add r5, r1,  -r3        ; r5 = AB
dp3 r5, r5, r5          ; r5 = |AB|^2
add r6, c17, -r1        ; r6 = BC
dp3 r6, r6, r6          ; r6 = |BC|^2

sge r8, r4, r5          ; |AC| > |AB|
mul r10.a, r10.a, r8.x
sge r8, r4, r6          ; |AC| > |BC|
mul r10.a, r10.a, r8.x

;;;;;;;;;;;;;;;;;;; Results: color ;;;;;;;;;;;;;;;;;;;;;;;;;;;
mov oD0, r10
