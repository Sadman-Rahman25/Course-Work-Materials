from OpenGL.GL import *
from OpenGL.GLUT import *
from OpenGL.GLU import *
import math, random, time

#Window & camera 
WINDOW_WIDTH, WINDOW_HEIGHT=1000,700
camera_r = math.hypot(0, 500)
camera_phi = math.pi / 2
camera_z = 500
camera_mode = 0
auto_camera = False
cheat_mode = False
cheat_1p = False

#Player & game state
player_pos = [0, 0, 0]
gun_angle = 90
player_lives = 5
missed_bullets = 0
score = 0
game_over = False

# Falling state
falling = False
fall_angle = 0.0
FALL_SPEED_DEG_PER_SEC = 200.0

# Entities
bullets = []
enemies = []

#Constants
GRID_LENGTH = 700
BULLET_SPEED = 5
BULLET_SIZE = 15
ENEMY_SPEED = 0.2
ENEMY_RADIUS = 30.0
MAX_ENEMIES = 5
MAX_MISSED = 10
CHEAT_ROT_SPEED = 0.45
FIRE_THRESHOLD = 0.2
WALL_THICKNESS = 10.0
PLAYER_BODY_HALF = 15.0
PLAYER_COLLISION_MARGIN = 4.0
PLAYER_RADIUS = PLAYER_BODY_HALF + PLAYER_COLLISION_MARGIN

# Timing
last_time = time.time()

# Quadrics
quad_sphere = None
quad_cyl = None

# Utilities 
def clampAngle(a):
    a = (a + 180) % 360 - 180
    return a

def drawBox(cx, cy, cz, hx, hy, hz):
    glBegin(GL_QUADS)
    # front
    glVertex3f(cx - hx, cy + hy, cz - hz)
    glVertex3f(cx + hx, cy + hy, cz - hz)
    glVertex3f(cx + hx, cy + hy, cz + hz)
    glVertex3f(cx - hx, cy + hy, cz + hz)
    # back
    glVertex3f(cx - hx, cy - hy, cz - hz)
    glVertex3f(cx - hx, cy - hy, cz + hz)
    glVertex3f(cx + hx, cy - hy, cz + hz)
    glVertex3f(cx + hx, cy - hy, cz - hz)
    # left
    glVertex3f(cx - hx, cy - hy, cz - hz)
    glVertex3f(cx - hx, cy - hy, cz + hz)
    glVertex3f(cx - hx, cy + hy, cz + hz)
    glVertex3f(cx - hx, cy + hy, cz - hz)
    # right
    glVertex3f(cx + hx, cy - hy, cz - hz)
    glVertex3f(cx + hx, cy - hy, cz + hz)
    glVertex3f(cx + hx, cy + hy, cz + hz)
    glVertex3f(cx + hx, cy + hy, cz - hz)
    # top
    glVertex3f(cx - hx, cy - hy, cz + hz)
    glVertex3f(cx + hx, cy - hy, cz + hz)
    glVertex3f(cx + hx, cy + hy, cz + hz)
    glVertex3f(cx - hx, cy + hy, cz + hz)
    # bottom
    glVertex3f(cx - hx, cy - hy, cz - hz)
    glVertex3f(cx - hx, cy + hy, cz - hz)
    glVertex3f(cx + hx, cy + hy, cz - hz)
    glVertex3f(cx + hx, cy - hy, cz - hz)
    glEnd()

#Spawners & Reset
def randomEnemy():
    edge = random.choice([0,1,2,3])
    pad = int(WALL_THICKNESS + ENEMY_RADIUS + 10)
    if edge == 0:
        x = random.uniform(-GRID_LENGTH + pad, GRID_LENGTH - pad)
        y = -GRID_LENGTH + pad
    elif edge == 1:
        x = random.uniform(-GRID_LENGTH + pad, GRID_LENGTH - pad)
        y = GRID_LENGTH - pad
    elif edge == 2:
        x = -GRID_LENGTH + pad
        y = random.uniform(-GRID_LENGTH + pad, GRID_LENGTH - pad)
    else:
        x = GRID_LENGTH - pad
        y = random.uniform(-GRID_LENGTH + pad, GRID_LENGTH - pad)
    return {'x': x, 'y': y, 'z': 20.0, 'phase': random.random() * 6.283}

def initEnemies():
    global enemies
    enemies = []  
    for i in range(MAX_ENEMIES):
        new_enemy = randomEnemy()  
        enemies.append(new_enemy)  


def resetGame():
    global bullets, player_lives, missed_bullets, score, gun_angle, camera_mode, auto_camera, game_over, player_pos, falling, fall_angle, last_time
    bullets.clear()
    player_lives = 5
    missed_bullets = 0
    score = 0
    player_pos = [0.0, 0.0, 0.0]
    gun_angle = 90.0
    camera_mode = 0
    auto_camera = False
    game_over = False
    falling = False
    fall_angle = 0.0
    last_time = time.time()
    initEnemies()

#  Drawing: Text & HUD 
def drawText(x, y, text, color=(1,1,1), font=GLUT_BITMAP_HELVETICA_18):
    glColor3f(float(color[0]), float(color[1]), float(color[2]))
    glMatrixMode(GL_PROJECTION)
    glPushMatrix()
    glLoadIdentity()
    gluOrtho2D(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT)
    glMatrixMode(GL_MODELVIEW)
    glPushMatrix()
    glLoadIdentity()
    glRasterPos2f(x, y)
    for ch in str(text):
        glutBitmapCharacter(font, ord(ch))
    glPopMatrix()
    glMatrixMode(GL_PROJECTION)
    glPopMatrix()
    glMatrixMode(GL_MODELVIEW)


def drawHudBox(x,y,w,h,color=(1,1,1)):
    glMatrixMode(GL_PROJECTION)
    glPushMatrix()
    glLoadIdentity()
    gluOrtho2D(0,WINDOW_WIDTH,0,WINDOW_HEIGHT)
    glMatrixMode(GL_MODELVIEW)
    glPushMatrix()
    glLoadIdentity()
    glColor3f(float(color[0]), float(color[1]), float(color[2]))
    glBegin(GL_QUADS)
    glVertex3f(x,y,0)
    glVertex3f(x+w,y,0)
    glVertex3f(x+w,y+h,0)
    glVertex3f(x,y+h,0)
    glEnd()
    glPopMatrix()
    glMatrixMode(GL_PROJECTION)
    glPopMatrix()
    glMatrixMode(GL_MODELVIEW)

# Drawing: Entities
def drawGridAndBoundaries():
    tile_size = 50
    half = GRID_LENGTH
    start = -half
    end = half
    for tx in range(start, end, tile_size):
        for ty in range(start, end, tile_size):
            i = int((tx - start) / tile_size)
            j = int((ty - start) / tile_size)
            if (i + j) % 2 == 0:
                glColor3f(1.0, 1.0, 1.0)
            else:
                glColor3f(0.7, 0.0, 0.7)
            glBegin(GL_QUADS)
            glVertex3f(tx, ty, 0)
            glVertex3f(tx + tile_size, ty, 0)
            glVertex3f(tx + tile_size, ty + tile_size, 0)
            glVertex3f(tx, ty + tile_size, 0)
            glEnd()
    wall_thickness = WALL_THICKNESS
    wall_height = 50.0
    glColor3f(0.0, 0.0, 1.0)
    cx = GRID_LENGTH + wall_thickness/2.0
    cy = 0.0
    cz = wall_height / 2.0
    hx = wall_thickness/2.0
    hy = (GRID_LENGTH * 2.0 + wall_thickness) / 2.0
    hz = wall_height / 2.0
    drawBox(cx, cy, cz, hx, hy, hz)
    glColor3f(0.0, 1.0, 0.0)
    cx = -GRID_LENGTH - wall_thickness/2.0
    drawBox(cx, cy, cz, hx, hy, hz)
    glColor3f(1.0, 1.0, 1.0)
    cx = 0.0
    cy = GRID_LENGTH + wall_thickness/2.0
    hx = (GRID_LENGTH * 2.0 + wall_thickness) / 2.0
    hy = wall_thickness/2.0
    drawBox(cx, cy, cz, hx, hy, hz)
    glColor3f(0.0, 1.0, 1.0)
    cy = -GRID_LENGTH - wall_thickness/2.0
    drawBox(cx, cy, cz, hx, hy, hz)


def drawPlayerModel(x, y, z):
    global quad_sphere, quad_cyl
    glPushMatrix()
    glColor3f(0.0, 0.0, 0.0)
    glTranslatef(x, y, z + 110)
    if quad_sphere is None:
        q = gluNewQuadric()
        gluSphere(q, 25, 12, 12)
    else:
        gluSphere(quad_sphere, 25, 12, 12)
    glPopMatrix()
    glPushMatrix()
    glColor3f(0.0, 0.8, 0.0)
    glTranslatef(x, y, z + 45)
    glutSolidCube(30)
    glTranslatef(0, 0, 30)
    glutSolidCube(30)
    glPopMatrix()
    # left hand
    glPushMatrix()
    glColor3f(242/255.0,151/255.0,141/255.0)
    glTranslatef(x+20,y+0,z+80)
    glRotatef(-90,1,0,0)
    if quad_cyl is None:
        q = gluNewQuadric()
        gluCylinder(q,10,2,50,10,10)
    else:
        gluCylinder(quad_cyl,10,2,50,10,10)
    glPopMatrix()
    # right hand
    glPushMatrix()
    glColor3f(242/255.0,151/255.0,141/255.0)
    glTranslatef(x-20,y+0,z+80)
    glRotatef(-90,1,0,0)
    if quad_cyl is None:
        q = gluNewQuadric()
        gluCylinder(q,10,2,50,10,10)
    else:
        gluCylinder(quad_cyl,10,2,50,10,10)
    glPopMatrix()
    # gun barrel
    glPushMatrix()
    glColor3f(1.0,0.0,0.0)
    glTranslatef(x+0,y+0,z+80)
    glRotatef(-90,1,0,0)
    if quad_cyl is None:
        q = gluNewQuadric()
        gluCylinder(q,10,2,60,10,10)
    else:
        gluCylinder(quad_cyl,10,2,60,10,10)
    glPopMatrix()
    # left leg
    glPushMatrix()
    glColor3f(0.0,0.0,0.9)
    glTranslatef(x+10,y+0,z+30)
    glRotatef(-180,1,0,0)
    if quad_cyl is None:
        q = gluNewQuadric()
        gluCylinder(q,10,2,60,10,10)
    else:
        gluCylinder(quad_cyl,10,2,60,10,10)
    glPopMatrix()
    # right leg
    glPushMatrix()
    glColor3f(0.0,0.0,0.9)
    glTranslatef(x-10,y+0,z+30)
    glRotatef(-180,1,0,0)
    if quad_cyl is None:
        q = gluNewQuadric()
        gluCylinder(q,10,2,60,10,10)
    else:
        gluCylinder(quad_cyl,10,2,60,10,10)
    glPopMatrix()


def drawPlayer():
    glPushMatrix()
    glTranslatef(player_pos[0], player_pos[1], player_pos[2])
    glRotatef(gun_angle - 90, 0, 0, 1)
    drawPlayerModel(0, 0, 0)
    glPopMatrix()


def drawFallingPlayer():
    glPushMatrix()
    pivot_z = player_pos[2] + 60
    glTranslatef(player_pos[0], player_pos[1], pivot_z)
    glRotatef(fall_angle, 1, 0, 0)
    drawPlayerModel(0, 0, -60)
    glPopMatrix()


def drawEnemy(e):
    global quad_sphere
    s = 1.0 + 0.3 * math.sin(time.time()*3 + (e.get('phase') or 0))
    glPushMatrix()
    glTranslatef(e['x'], e['y'], e['z'])
    r = ENEMY_RADIUS * s
    glColor3f(1.0,0.0,0.0)
    if quad_sphere is None:
        q = gluNewQuadric()
        gluSphere(q, r, 20, 20)
        glColor3f(0.0,0.0,0.0)
        glTranslatef(0,0,r)
        gluSphere(q, r*0.6, 12, 12)
    else:
        gluSphere(quad_sphere, r, 20, 20)
        glColor3f(0.0,0.0,0.0)
        glTranslatef(0,0,r)
        gluSphere(quad_sphere, r*0.6, 12, 12)
    glPopMatrix()


def drawBullet(b):
    glPushMatrix()
    glTranslatef(b['x'], b['y'], b['z'])
    glColor3f(1.0,0.0,0.0)
    glutSolidCube(BULLET_SIZE)
    glPopMatrix()

#Game_Logic 
def fireBullet():
    rad = math.radians(gun_angle)
    muzzle_dist = 40
    x = player_pos[0] + math.cos(rad) * muzzle_dist
    y = player_pos[1] + math.sin(rad) * muzzle_dist
    bullets.append({'x':x,'y':y,'z':player_pos[2]+60,'angle':gun_angle})

def updateFalling(dt):
    global fall_angle, falling
    if falling and fall_angle < 90.0:
        fall_angle += FALL_SPEED_DEG_PER_SEC*dt
        if fall_angle >= 90.0:
            fall_angle = 90.0
            falling = False

def updateGame():
    global missed_bullets, player_lives, score, game_over, gun_angle, falling, last_time
    new_bullets = []
    for b in bullets:
        rad = math.radians(b['angle'])
        b['x'] += BULLET_SPEED*math.cos(rad)
        b['y'] += BULLET_SPEED*math.sin(rad)
        if abs(b['x']) < GRID_LENGTH and abs(b['y']) < GRID_LENGTH:
            new_bullets.append(b)
        else:
            missed_bullets += 1
    bullets[:] = new_bullets
    for e in enemies:
        dx,dy = player_pos[0]-e['x'], player_pos[1]-e['y']
        dist = math.hypot(dx,dy)
        if dist>0:
            e['x'] += ENEMY_SPEED*dx/dist
            e['y'] += ENEMY_SPEED*dy/dist
    for b in bullets[:]:
        hit = False
        for e in enemies:
            r = ENEMY_RADIUS*(1+0.3*math.sin(time.time()*3+(e.get('phase') or 0)))
            if math.hypot(b['x']-e['x'], b['y']-e['y']) < (r + BULLET_SIZE/2.0):
                try:
                    bullets.remove(b)
                except ValueError:
                    pass
                score += 1
                try:
                    enemies.remove(e)
                except ValueError:
                    pass
                enemies.append(randomEnemy())
                hit = True
                break
        if hit:
            continue
    if not game_over:
        for e in enemies[:]:
            r = ENEMY_RADIUS*(1+0.3*math.sin(time.time()*3+(e.get('phase') or 0)))
            if math.hypot(e['x']-player_pos[0], e['y']-player_pos[1]) < (r+PLAYER_RADIUS):#COLLISION (PLAYER+ENEMY)
                player_lives -= 1
                try:
                    enemies.remove(e)
                except ValueError:
                    pass
                enemies.append(randomEnemy())
    if cheat_mode and not game_over:
        gun_angle = (gun_angle+CHEAT_ROT_SPEED)%360
        for e in enemies:
            ang = math.degrees(math.atan2(e['y']-player_pos[1], e['x']-player_pos[0]))
            if abs(clampAngle(ang-gun_angle)) < FIRE_THRESHOLD:
                fireBullet()
                break
    if player_lives<=0 or missed_bullets>=MAX_MISSED:
        if not game_over:
            game_over=True
            falling=True
            last_time=time.time()

#Camera 
def setupCamera():
    global cheat_1p
    glMatrixMode(GL_PROJECTION)
    glLoadIdentity()
    gluPerspective(120, WINDOW_WIDTH/WINDOW_HEIGHT, 0.1, 2000)
    glMatrixMode(GL_MODELVIEW)
    glLoadIdentity()
    if cheat_mode and not auto_camera and camera_mode==1:
        rad = math.radians(gun_angle)
        ex = player_pos[0]-math.cos(rad)*10
        ey = player_pos[1]-math.sin(rad)*10+40
        ez = player_pos[2]+90
        cx = player_pos[0]+math.cos(rad)*100
        cy = player_pos[1]+math.sin(rad)*100
        gluLookAt(ex,ey,ez,cx,cy,ez,0,0,1)
        cheat_1p=True
    elif cheat_mode and auto_camera:
        rad=math.radians(gun_angle)
        ex = player_pos[0]-math.cos(rad)*10
        ey = player_pos[1]-math.sin(rad)*10+40
        ez = player_pos[2]+90
        cx = player_pos[0]+math.cos(rad)*100
        cy = player_pos[1]+math.sin(rad)*100
        gluLookAt(ex,ey,ez,cx,cy,ez,0,0,1)
        cheat_1p=False
    elif camera_mode==1:
        rad=math.radians(gun_angle)
        ex = player_pos[0]-math.cos(rad)*10
        ey = player_pos[1]-math.sin(rad)*10+40
        ez = player_pos[2]+90
        cx = player_pos[0]+math.cos(rad)*100
        cy = player_pos[1]+math.sin(rad)*100
        gluLookAt(ex,ey,ez,cx,cy,ez,0,0,1)
        cheat_1p=False
    else:
        x = camera_r*math.cos(camera_phi)
        y = camera_r*math.sin(camera_phi)-100
        gluLookAt(x,y,camera_z,0,0,0,0,0,1)
        cheat_1p=False

def showScreen():
    global last_time
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
    glViewport(0,0,WINDOW_WIDTH,WINDOW_HEIGHT)
    setupCamera()
    now=time.time()
    dt=now-last_time
    last_time=now
    if not game_over:
        updateGame()
    updateFalling(dt)
    drawGridAndBoundaries()
    for e in enemies:
        drawEnemy(e)
    for b in bullets:
        drawBullet(b)
    # Player 
    if game_over:
        box_w = 520
        box_h = 120
        box_x = (WINDOW_WIDTH - box_w) / 2
        box_y = (WINDOW_HEIGHT - box_h) / 2
        drawHudBox(box_x, box_y, box_w, box_h, color=(0.95,0.95,0.95))
        drawText(WINDOW_WIDTH / 2 - 120, WINDOW_HEIGHT / 2 + 40, "!!!   GAME OVER   !!!", color=(0.0, 0.0, 0.0))
        drawText(WINDOW_WIDTH / 2 - 120, WINDOW_HEIGHT / 2 + 10, "PRESS 'R' TO RESTART", color=(0.0, 0.0, 0.0))
        if fall_angle < 90.0:
            drawFallingPlayer()
        else:
            glPushMatrix()
            glTranslatef(player_pos[0], player_pos[1], player_pos[2])
            glRotatef(90, 1, 0, 0)
            drawPlayer()
            glPopMatrix()
    else:
        drawPlayer()
    drawText(10, WINDOW_HEIGHT - 20, f"Lives: {player_lives}   Score: {score}   Missed: {missed_bullets}")
    drawText(10, WINDOW_HEIGHT - 50, f"Cheat [C]: {cheat_mode}   Follow [V]: {auto_camera}")
    glutSwapBuffers()

def idle():
    glutPostRedisplay()

# Input 
def keyboardListener(key, x, y):
    global cheat_mode, auto_camera, gun_angle, game_over, player_pos, falling, fall_angle
    if isinstance(key, bytes):
        try:
            k = key.decode('utf-8')
        except:
            k = chr(key) if isinstance(key, int) else str(key)
    else:
        k = str(key)
    k = k.lower()
    if not game_over:
        min_coord = -GRID_LENGTH + WALL_THICKNESS + PLAYER_RADIUS
        max_coord = GRID_LENGTH - WALL_THICKNESS - PLAYER_RADIUS
        if k == 'w':
            rad = math.radians(gun_angle)
            player_pos[0] += 5 * math.cos(rad)
            player_pos[1] += 5 * math.sin(rad)
        elif k == 's':
            rad = math.radians(gun_angle)
            player_pos[0] -= 5 * math.cos(rad)
            player_pos[1] -= 5 * math.sin(rad)
        elif k == 'a':
            gun_angle = (gun_angle + 5) % 360
        elif k == 'd':
            gun_angle = (gun_angle - 5) % 360
        elif k == 'c':
            cheat_mode = not cheat_mode
        elif k == 'v' and cheat_mode:
            auto_camera = not auto_camera
        player_pos[0] = max(min_coord, min(max_coord, player_pos[0]))
        player_pos[1] = max(min_coord, min(max_coord, player_pos[1]))
    if k == 'r' and game_over:
        resetGame()

def specialKeyListener(key, x, y):
    global camera_phi, camera_z
    if key == GLUT_KEY_UP:
        camera_z += 20
    if key == GLUT_KEY_DOWN:
        camera_z = max(50, camera_z - 20)
    if key == GLUT_KEY_LEFT:
        camera_phi -= 0.05
    if key == GLUT_KEY_RIGHT:
        camera_phi += 0.05

def mouseListener(button, state, x, y):
    global camera_mode
    if button == GLUT_LEFT_BUTTON and state == GLUT_DOWN and not game_over:
        fireBullet()
    if button == GLUT_RIGHT_BUTTON and state == GLUT_DOWN:
        camera_mode = 1 - camera_mode

#Entry Point
def main():
    global last_time, quad_sphere, quad_cyl
    glutInit()
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH)
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT)
    glutInitWindowPosition(0, 0)
    glutCreateWindow(b"Bullet Frenzy")
    quad_sphere = gluNewQuadric()
    quad_cyl = gluNewQuadric()
    glClearColor(0.0, 0.0, 0.0, 1.0)
    resetGame()
    last_time = time.time()
    glutDisplayFunc(showScreen)
    glutKeyboardFunc(keyboardListener)
    glutSpecialFunc(specialKeyListener)
    glutMouseFunc(mouseListener)
    glutIdleFunc(idle)
    glutMainLoop()

if __name__ == "__main__":
    main()
