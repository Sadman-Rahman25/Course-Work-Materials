import random
from OpenGL.GL import*
from OpenGL.GLU import*
from OpenGL.GLUT import*

WINDOW_WIDTH=500
WINDOW_HEIGHT=500
CATCHER_WIDTH = 100
catcher_pos_x,catcher_pos_y=0,20
diamond_pos_x,diamond_pos_y=245,430
move_left,move_right=0,0
score=0
paused=False
game_ended=False
diamond_velocity=0.25
start_time=0
diamond_color=(random.uniform(0.5, 1.0),random.uniform(0.5, 1.0),random.uniform(0.5, 1.0))

#MIDPOINT ALGO
def get_zone(x0,y0,x1,y1):
    dx=x1-x0
    dy=y1-y0
    if abs(dx)>=abs(dy):
        if dx>0 and dy>0:return 0
        if dx<0 and dy>0:return 3
        if dx<0 and dy<0:return 4
        return 7
    else:
        if dx>0 and dy>0:return 1
        if dx<0 and dy>0:return 2
        if dx<0 and dy<0:return 5
        return 6

def to_zone0(zone,x,y):
    zone_map=[(x,y),(y,x),(y,-x),(-x,y),(-x,-y),(-y,-x),(-y,x),(x,-y)]
    return zone_map[zone]

def from_zone0(zone,x,y):
    zone_map=[(x,y),(y,x),(-y,x),(-x,y),(-x,-y),(-y,-x),(y,-x),(x,-y)]
    return zone_map[zone]

def draw_pixel(x,y):
    glPointSize(3)
    glBegin(GL_POINTS)
    glVertex2f(x,y)
    glEnd()

def draw_line(x0,y0,x1,y1):
    zone=get_zone(x0,y0,x1,y1)
    x0,y0=to_zone0(zone,x0,y0)
    x1,y1=to_zone0(zone,x1,y1)
    dx,dy=x1-x0,y1-y0
    dEast,dNEast=2*dy,2*(dy-dx)
    d=2*dy-dx
    while x0<=x1:
        ox,oy=from_zone0(zone,x0,y0)
        draw_pixel(ox,oy)
        if d<0:
            d+=dEast
            x0+=1
        else:
            d+=dNEast
            x0+=1
            y0+=1

#OBJECTS
def catcher():
    glColor3f(0.9, 0.9, 0.9) if not game_ended else glColor3f(1.0, 0.2, 0.2)
    base_x=catcher_pos_x-move_left+move_right
    base_x=max(0,min(base_x, 500-CATCHER_WIDTH))
    top_y=catcher_pos_y
    bottom_y=catcher_pos_y-12
    draw_line(base_x,top_y,base_x+CATCHER_WIDTH,top_y)
    draw_line(base_x,top_y,base_x+10,bottom_y)
    draw_line(base_x+90,bottom_y,base_x+CATCHER_WIDTH,top_y)
    draw_line(base_x+10,bottom_y,base_x+90,bottom_y)

def diamond():
    glColor3f(*diamond_color)
    center_x = diamond_pos_x + 14  
    center_y = diamond_pos_y
    top_y = center_y + 16
    bottom_y = center_y - 16
    left_x = center_x - 14
    right_x = center_x + 14
    draw_line(center_x, top_y, left_x, center_y)
    draw_line(center_x, top_y, right_x, center_y)
    draw_line(center_x, bottom_y, left_x, center_y)
    draw_line(center_x, bottom_y, right_x, center_y)

def exit_icon():
    glColor3f(1.0,0.0,0.0)
    draw_line(450,485,480,455)
    draw_line(450,455,480,485)

def restart_icon():
    glColor3f(0.0, 1.0, 0.9) 
    draw_line(8,472,42,472)
    draw_line(8,472,22,458)
    draw_line(8,472,22,486)

def pause_icon():
    if not paused:
        glColor3f(1.0,0.5,0.0)
    else:
        glColor3f(0.0,0.5,0.3)
    if not paused:
        draw_line(255,488,255,458)
        draw_line(267,488,267,458)
    else:
        draw_line(255,492,255,454)
        draw_line(255,492,273,473)
        draw_line(255,454,273,473)

def check_catch():
    global diamond_pos_x,diamond_pos_y,score,diamond_color
    if not paused and not game_ended:
        base=catcher_pos_x-move_left+move_right
        catcher_top=catcher_pos_y
        catcher_bottom=catcher_pos_y-18
        diamond_top=diamond_pos_y+20
        diamond_bottom=diamond_pos_y-20
        diamond_center_x=diamond_pos_x+15
        vertical_overlap=diamond_bottom<=catcher_top and diamond_top>=catcher_bottom
        horizontal_overlap=base<=diamond_center_x<=base+100
        if vertical_overlap and horizontal_overlap:
            score+=1
            print(f"Score:{score}")
            diamond_pos_x=random.randint(5,WINDOW_WIDTH-33)
            diamond_pos_y=WINDOW_HEIGHT-70
            diamond_color=(random.random(),random.random(),random.random())

def restart_game():
    global catcher_pos_x,catcher_pos_y,diamond_pos_x,diamond_pos_y
    global move_left,move_right,score,diamond_velocity,diamond_color
    global game_ended,paused,start_time
    catcher_pos_x,catcher_pos_y=0,30
    diamond_pos_x = random.randint(5, WINDOW_WIDTH - 33)
    diamond_pos_y = 450
    move_left=move_right=100
    score=0
    diamond_velocity=0.30
    diamond_color=(random.random(),random.random(),random.random())
    game_ended=paused=False
    start_time=glutGet(GLUT_ELAPSED_TIME)/1000
    print("Game restarted.")

def on_mouse(button,state,x,y):
    global paused
    if button==GLUT_LEFT_BUTTON and state==GLUT_DOWN:
        click_y=WINDOW_HEIGHT-y
        restart_left=int(0.01*WINDOW_WIDTH)
        restart_right=int(0.08*WINDOW_WIDTH)
        restart_bottom=int(0.92*WINDOW_HEIGHT)
        restart_top=int(0.98*WINDOW_HEIGHT)
        pause_left=int(0.49*WINDOW_WIDTH)
        pause_right=int(0.56*WINDOW_WIDTH)
        pause_bottom=int(0.91*WINDOW_HEIGHT)
        pause_top=int(0.99*WINDOW_HEIGHT)
        exit_left=int(0.90*WINDOW_WIDTH)
        exit_right=int(0.96*WINDOW_WIDTH)
        exit_bottom=int(0.92*WINDOW_HEIGHT)
        exit_top=int(0.98*WINDOW_HEIGHT)
        if restart_left<=x<=restart_right and restart_bottom<=click_y<=restart_top:
            restart_game()
        elif pause_left<=x<=pause_right and pause_bottom<=click_y<=pause_top:
            paused=not paused
        elif exit_left<=x<=exit_right and exit_bottom<=click_y<=exit_top:
            print(f"Goodbye! Final Score:{score}")
            glutLeaveMainLoop()

def on_keys(key,x,y):
    global move_left,move_right
    base=catcher_pos_x-move_left+move_right
    if not paused and not game_ended:
        if key==GLUT_KEY_LEFT and base>0:
            move_left+=10
        elif key==GLUT_KEY_UP and base+100<500:
            move_right+=10

def update_game():
    global diamond_pos_y,game_ended,diamond_velocity,start_time
    if not paused and not game_ended:
        time_elapsed=(glutGet(GLUT_ELAPSED_TIME)/1000)-start_time
        diamond_velocity=0.25+time_elapsed*0.1
        diamond_pos_y-=diamond_velocity
        if diamond_pos_y+20<=0:
            game_ended=True
            print(f"Game Over! Final Score:{score}")
    if not game_ended:
        check_catch()

def setup_2d():
    glViewport(0,0,WINDOW_WIDTH,WINDOW_HEIGHT)
    glMatrixMode(GL_PROJECTION)
    glLoadIdentity()
    glOrtho(0.0,WINDOW_WIDTH,0.0,WINDOW_HEIGHT,0.0,1.0)
    glMatrixMode(GL_MODELVIEW)
    glLoadIdentity()

def render():
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT)
    glLoadIdentity()
    setup_2d()
    exit_icon()
    pause_icon()
    restart_icon()
    diamond()
    catcher()
    update_game()
    glutSwapBuffers()

if __name__=='__main__':
    glutInit()
    glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_DEPTH)
    glutInitWindowSize(WINDOW_WIDTH,WINDOW_HEIGHT)
    glutInitWindowPosition(100,100)
    glutCreateWindow(b"Catch the Diamonds!")
    glutDisplayFunc(render)
    glutIdleFunc(render)
    glutMouseFunc(on_mouse)
    glutSpecialFunc(on_keys)
    glutMainLoop()
