import tkinter as tk
import math
import random
import time
from typing import List, Tuple

class Triangle:

    
    TYPES = {
        'small': {'size': 20, 'speed': (2, 4), 'destroy_time': 0.8},
        'medium': {'size': 30, 'speed': (1.5, 3), 'destroy_time': 1.2},
        'large': {'size': 40, 'speed': (1, 2), 'destroy_time': 1.6},
    }
    
    def __init__(self, canvas_width: int, canvas_height: int):
        self.canvas_width = canvas_width
        self.canvas_height = canvas_height
        
        triangle_type = random.choice(['small', 'medium', 'large'])
        config = self.TYPES[triangle_type]
        
        self.size = config['size']
        self.type = triangle_type
        self.destroy_threshold = config['destroy_time']
        self.color = '#ffffff'  
        
        self._init_position_and_speed(config['speed'])
        
        self.exposure_time = 0
        self.is_destroyed = False
        
    def _init_position_and_speed(self, speed_range: Tuple[float, float]):
        side = random.choice(['left', 'right', 'top', 'bottom'])
        min_speed, max_speed = speed_range
        
        if side == 'left':
            self.x = -self.size
            self.y = random.randint(self.size, self.canvas_height - self.size)
            self.speed_x = random.uniform(min_speed, max_speed)
            self.speed_y = random.uniform(-0.5, 0.5)
            self.direction = 0
        elif side == 'right':
            self.x = self.canvas_width + self.size
            self.y = random.randint(self.size, self.canvas_height - self.size)
            self.speed_x = random.uniform(-max_speed, -min_speed)
            self.speed_y = random.uniform(-0.5, 0.5)
            self.direction = math.pi
        elif side == 'top':
            self.x = random.randint(self.size, self.canvas_width - self.size)
            self.y = -self.size
            self.speed_x = random.uniform(-0.5, 0.5)
            self.speed_y = random.uniform(min_speed, max_speed)
            self.direction = math.pi / 2
        else:  
            self.x = random.randint(self.size, self.canvas_width - self.size)
            self.y = self.canvas_height + self.size
            self.speed_x = random.uniform(-0.5, 0.5)
            self.speed_y = random.uniform(-max_speed, -min_speed)
            self.direction = -math.pi / 2
            
    def update(self):
        if not self.is_destroyed:
            self.x += self.speed_x
            self.y += self.speed_y
            
            if abs(self.speed_x) > 0.1 or abs(self.speed_y) > 0.1:
                self.direction = math.atan2(self.speed_y, self.speed_x)
                
    def is_off_screen(self) -> bool:
        buffer = self.size * 3
        return (self.x < -buffer or self.x > self.canvas_width + buffer or 
                self.y < -buffer or self.y > self.canvas_height + buffer)
                
    def reset_exposure(self):
        self.exposure_time = 0
        
    def increase_exposure(self, delta_time: float):
        if not self.is_destroyed:
            self.exposure_time += delta_time
            if self.exposure_time >= self.destroy_threshold:
                self.is_destroyed = True

class TriangleGame:
    
    def __init__(self):
        self.root = tk.Tk()
        self.root.title("Треугольники")
        self.root.resizable(False, False)
        
        self.CANVAS_WIDTH = 900
        self.CANVAS_HEIGHT = 600
        
        self.TRIANGLE_SPAWN_RATE = 0.04
        self.MAX_TRIANGLES = 12
        self.CURSOR_RADIUS = 20  
        
        self.canvas = tk.Canvas(
            self.root, 
            width=self.CANVAS_WIDTH, 
            height=self.CANVAS_HEIGHT, 
            bg='#1a4b8c',  
            highlightthickness=0
        )
        self.canvas.pack()
        
        self.triangles: List[Triangle] = []
        self.last_time = time.time()
        self.is_paused = False
        
        self.cursor = self.canvas.create_oval(
            0, 0, 0, 0, 
            fill='yellow', outline='', tags='cursor'
        )
        
        self.setup_bindings()
        
        self.game_loop()
        
    def setup_bindings(self):
        self.canvas.bind('<Motion>', self.on_mouse_move)
        self.canvas.bind('<Enter>', lambda e: self.canvas.focus_set())
        self.root.bind('<KeyPress>', self.on_key_press)
        self.root.bind('<space>', self.toggle_pause)
        
    def on_mouse_move(self, event):
        if self.is_paused:
            return
            
        x, y = event.x, event.y
        self.canvas.coords(
            self.cursor,
            x - self.CURSOR_RADIUS, y - self.CURSOR_RADIUS,
            x + self.CURSOR_RADIUS, y + self.CURSOR_RADIUS
        )
        
    def on_key_press(self, event):
        if event.keysym == 'Escape':
            self.root.quit()
        elif event.keysym == 'space':
            self.toggle_pause()
            
    def toggle_pause(self, event=None):
        self.is_paused = not self.is_paused
        if not self.is_paused:
            self.last_time = time.time()
            
    def spawn_triangle(self):
        if len(self.triangles) < self.MAX_TRIANGLES and random.random() < self.TRIANGLE_SPAWN_RATE:
            new_triangle = Triangle(self.CANVAS_WIDTH, self.CANVAS_HEIGHT)
            self.triangles.append(new_triangle)
            
    def update_triangles(self, delta_time: float):
        cursor_coords = self.canvas.coords(self.cursor)
        if not cursor_coords:
            return
            
        cursor_x = (cursor_coords[0] + cursor_coords[2]) / 2
        cursor_y = (cursor_coords[1] + cursor_coords[3]) / 2
        
        for triangle in self.triangles[:]:
            triangle.update()
            
            if triangle.is_off_screen():
                self.triangles.remove(triangle)
                continue
                
            if self.is_cursor_hitting_triangle(cursor_x, cursor_y, triangle):
                triangle.increase_exposure(delta_time)
            else:
                triangle.reset_exposure()
                
    def is_cursor_hitting_triangle(self, cursor_x: float, cursor_y: float, triangle: Triangle) -> bool:
        distance = math.sqrt((cursor_x - triangle.x)**2 + (cursor_y - triangle.y)**2)
        
        return distance <= (self.CURSOR_RADIUS + triangle.size)
        
    def draw_triangles(self):
        self.canvas.delete('triangle')
        
        for triangle in self.triangles:
            if triangle.is_destroyed:
                self.triangles.remove(triangle)
                continue
                
            self.draw_triangle(triangle.x, triangle.y, triangle.size, triangle.color, triangle.direction)
                
    def draw_triangle(self, x: float, y: float, size: float, color: str, direction: float):
        cos_dir = math.cos(direction)
        sin_dir = math.sin(direction)
        
        nose_x = x + cos_dir * size
        nose_y = y + sin_dir * size
        
        left_x = x - cos_dir * size * 0.5 - sin_dir * size * 0.7
        left_y = y - sin_dir * size * 0.5 + cos_dir * size * 0.7
        
        right_x = x - cos_dir * size * 0.5 + sin_dir * size * 0.7
        right_y = y - sin_dir * size * 0.5 - cos_dir * size * 0.7
        
        points = [nose_x, nose_y, left_x, left_y, right_x, right_y]
        
        self.canvas.create_polygon(points, fill=color, outline='#cccccc', width=1, tags='triangle')
        
    def game_loop(self):
        if self.is_paused:
            self.root.after(16, self.game_loop)
            return
            
        current_time = time.time()
        delta_time = current_time - self.last_time
        self.last_time = current_time
        
        delta_time = min(delta_time, 0.1)
        
        self.spawn_triangle()
        self.update_triangles(delta_time)
        self.draw_triangles()
        
        self.root.after(16, self.game_loop)
        
    def run(self):
        try:
            self.root.mainloop()
        except KeyboardInterrupt:
            self.root.quit()


if __name__ == "__main__":
    
    
    game = TriangleGame()
    game.run()
