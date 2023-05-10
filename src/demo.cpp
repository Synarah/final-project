// Bryn Mawr College, alinen, 2020
//

#include <cmath>
#include <string>
#include <vector>
#include "agl/window.h"
#include <functional>
#include <string>
#include <random>
#include <unistd.h>



using namespace std;
using namespace glm;
using namespace agl;




class Shooter {
public:
  vec2 position;
  vec2 size;
  float speed;
  vec4 color;
};

class Alien {
public:
  Alien(vec2 pos, vec2 s, bool alive){
    position = pos;
    size = s;
    isalive = alive;
  }
  vec2 position;
  vec2 size;
  bool isalive;
};


struct Particle {
  glm::vec3 pos;
  glm::vec3 vel;
  glm::vec4 color;
  float rot;
  float size;
  float life;
  bool dead = false;
  float radius;
};



class Viewer : public Window {
public:


  Shooter shooter;

  std::vector<Alien> aliens;

  bool gameover = false;
  bool bulletlost = false;
  bool restartrequested = false;

  bool lastalienhit = false;

  Viewer() : Window() {
  }

  void setup() {
    setWindowSize(1000, 1000);
    renderer.blendMode(agl::BLEND);
    renderer.loadShader("texture", "../shaders/texture.vs", "../shaders/texture.fs");
    renderer.loadTexture("bullet", "../textures/bullet2.png", 0);
    renderer.loadTexture("shooter", "../textures/shooter2.png", 0);
    renderer.loadTexture("alien", "../textures/alien.png", 0);
    renderer.loadTexture("meteor", "../textures/meteor.png", 0);
    renderer.loadTexture("heart", "../textures/heart2.png", 0);
    renderer.loadTexture("gameover", "../textures/gameover2.png", 0); 
    renderer.loadTexture("background", "../textures/background.png",0);
    renderer.loadTexture("win", "../textures/win.png", 0);
    meteor.pos = (agl::randomUnitCube() - 0.5f);
    meteor.pos.y = 0.6;
    meteor.pos.z = 0;
    meteor.vel = vec3(0, -0.01, 0);
    meteor.radius = 0.09;
    shooter.color = vec4(1.0, 1.0, 0.8, 1.0);
    bullet.radius = 0.09f;
    shooter.position = vec2(0, -0.9);
    shooter.size = vec2(0.3, 0.05);
    shooter.speed = 0.5f;
    shooter.color = vec4(0.0f, 1.0f, 1.0f, 1.0f);
    const float alienwidth = 0.1f;
    const float alienheight = 0.05f;
    const float aliengap = 0.01f;
    const float aliensstartx = -0.45f;
    const float aliensstarty = 0.5f;
    for (int i = 0; i < numaliens; i++) {
      aliens.push_back(Alien(
        vec2(
          aliensstartx + (alienwidth + aliengap) * (i % 10),
          aliensstarty - (alienheight + aliengap) * (i / 10)
        ),
        vec2(alienwidth, alienheight),
        true
      ));
    }
  }

  void updateBullet(){
    if(!bullet.dead && shot){
      bullet.pos.y += 0.01;
    }
    else if (!bullet.dead && !shot){
      bullet.pos.x = shooter.position.x;
      bullet.pos.y = shooter.position.y - 0.1;
    }
    else {
      shot = false;
      bullet.dead = false;
      bullet.pos.x = shooter.position.x;
      bullet.pos.y = shooter.position.y - 0.1;
    }
  }

  void restartGame() {
    alienVel.y = 0;
    level = 1;
    hitcount = 0;
    aliens.clear();
    numaliens = 10;
    const float alienwidth = 0.1f;
    const float alienheight = 0.05f;
    const float aliengap = 0.01f;
    const float aliensstartx = -0.45f;
    const float aliensstarty = 0.5f;
    for (int i = 0; i < numaliens; i++) {
      aliens.push_back(Alien(
        vec2(
        aliensstartx + (alienwidth + aliengap) * (i % 10),
        aliensstarty - (alienheight + aliengap) * (i / 10)
      ),
      vec2(alienwidth, alienheight),
      true
      ));
  }
  score = 0;
  gameover = true;
}

void nextLevel() {
    aliens.clear();
    const int numaliens = level * 10;
    const float alienwidth = 0.1f;
    const float alienheight = 0.05f;
    const float aliengap = 0.01f;
    const float aliensstartx = -0.45f;
    const float aliensstarty = 0.5f;
    for (int i = 0; i < numaliens; i++) {
      float space = alienheight * (i / 10);
      aliens.push_back(Alien(
        vec2(
        (aliensstartx + (alienwidth + aliengap) * (i % 10)),
        (aliensstarty - (alienheight + aliengap) + space*2
        )
      ),
      vec2(alienwidth, alienheight),
      true
      ));
  }
}



  bool checkBulletAlienCollision() {
    for (int i = 0; i < aliens.size(); i++) {
      if (aliens[i].isalive &&
          bullet.pos.y + bullet.radius > aliens[i].position.y - aliens[i].size.y / 2 &&
          bullet.pos.y - bullet.radius < aliens[i].position.y + aliens[i].size.y / 2 &&
          bullet.pos.x > aliens[i].position.x - aliens[i].size.x / 2 &&
          bullet.pos.x < aliens[i].position.x + aliens[i].size.x / 2) {
            bullet.dead = true;
            aliens[i].isalive = false;
            score++;
         if (i == aliens.size() - 1) {
    lastalienhit = true;
  }
        return true;
      }
    }
    if(bullet.pos.y > 0.9f){
      bullet.dead = true;
    }
    return false;
  }


    bool checkShooterAlienCollision() {
    for (int i = 0; i < aliens.size(); i++) {
      if(aliens[i].isalive && aliens[i].position.y <= shooter.position.y){
        gameover = true;
      }
      if (aliens[i].isalive &&
          shooter.position.y + shooter.size.y > aliens[i].position.y - aliens[i].size.y / 2 &&
          shooter.position.y - shooter.size.y < aliens[i].position.y + aliens[i].size.y / 2 &&
          shooter.position.x > aliens[i].position.x - aliens[i].size.x / 2 &&
          shooter.position.x < aliens[i].position.x + aliens[i].size.x / 2) {
            gameover = true;
        return true;
      }
    }
    return false;
  }


  void keyEvent(int key, double dt) {
  if (key == GLFW_KEY_LEFT) {
    shooter.position.x -= shooter.speed * dt;
  } else if (key == GLFW_KEY_RIGHT) {
    shooter.position.x += shooter.speed * dt;
  }
}

void keyDown(int key, int action){
  if (key == GLFW_KEY_UP) {
    shot = true;
    }
}

void drawScore(){

}

void updateAliens(float dt){
  timePassed += dt;
  if(timePassed > 1){
    timePassed = 0;
    if(aliens[0].position.x < -0.5){
      alienVel.x = 0.01;
    }
    else if(aliens[aliens.size() - 1].position.x >= 0.53){
      alienVel.x = -0.01;
    }
    for(int i = 0; i < aliens.size(); i++){
      aliens[i].position.x += alienVel.x;
      aliens[i].position.y += alienVel.y;
    }
  }
}

void updateShooterPosition(float dt) {
  float newXPos = shooter.position.x;
  float leftBound = -1.0f + shooter.size.x / 2;
  float rightBound = 1.0f - shooter.size.x / 2;
  if (glfwGetKey(window(), GLFW_KEY_LEFT) == GLFW_PRESS) {
    newXPos = shooter.position.x - shooter.speed * dt;
  } else if (glfwGetKey(window(), GLFW_KEY_RIGHT) == GLFW_PRESS) {
    newXPos = shooter.position.x + shooter.speed * dt;
  }
  if (newXPos >= leftBound && newXPos <= rightBound) {
    shooter.position.x = newXPos;
  }
}

  void drawBullet(){
    renderer.beginShader("sprite");
    renderer.texture("image", "bullet");
    renderer.push();
    renderer.scale(vec3(0.3, 1, 1));
    renderer.pop();
    renderer.sprite(bullet.pos, vec4(1.0), .02);
  }

  void drawShooter(){
    renderer.beginShader("sprite");
    renderer.texture("image", "shooter");
    renderer.sprite(vec3(shooter.position, 0), vec4(1.0), 0.1);
    renderer.endShader();
  }

  void drawAliens(){
    renderer.beginShader("sprite");
    renderer.texture("image", "alien");
    for (int i = 0; i < aliens.size(); i++) {
      if (aliens[i].isalive) {
        renderer.sprite(vec3(aliens[i].position, 0), vec4(1.0), 0.1);
    }
  }
  }

  void updateMeteor(){
    if(meteorShooterCollision() || meteor.pos.y <= -1.0){
      meteor.pos.x = agl::randomUnitCube().x;
      meteor.pos.y = 1.0;
    }
    else if(meteorBulletCollision()){
      meteor.pos.x = agl::randomUnitCube().x;
      meteor.pos.y = 1.0;
    }
    else{
      meteor.pos += meteor.vel;
    }
  }
  //bigger, two hits to kill
  void drawMeteor(){
    renderer.beginShader("sprite");
    renderer.texture("image", "meteor");
    renderer.sprite(meteor.pos, vec4(1.0), 0.1);
    renderer.endShader();
  }

  bool meteorBulletCollision(){
    bool hit = false;
      if (meteor.pos.y + meteor.radius > bullet.pos.y - bullet.radius / 2 &&
          meteor.pos.y - meteor.radius < bullet.pos.y + bullet.radius / 2 &&
          meteor.pos.x > bullet.pos.x - bullet.radius / 2 &&
          meteor.pos.x < bullet.pos.x + bullet.radius / 2) {
            hit = true;
            bullet.dead = true;
            if(hitcount >0){
              hitcount--;
            }
          }
    return hit;
  }

  bool meteorShooterCollision(){
    bool hit = false;
      if (meteor.pos.y + meteor.radius > shooter.position.y - shooter.size.y / 2 &&
          meteor.pos.y - meteor.radius < shooter.position.y + shooter.size.y / 2 &&
          meteor.pos.x > shooter.position.x - shooter.size.x / 2 &&
          meteor.pos.x < shooter.position.x + shooter.size.x / 2) {
            hit = true;
            hitcount++;
          }
    return hit;
  }

  void drawLives(){
    renderer.beginShader("sprite");
    renderer.texture("image", "heart");
    renderer.sprite(vec3(0.6, 0.97, 0), vec4(1.0), 0.1);
    if(hitcount < 4){
      renderer.sprite(vec3(0.69, 0.97, 0), vec4(1.0), 0.1);
      if(hitcount < 3){
        renderer.sprite(vec3(0.78, 0.97, 0), vec4(1.0), 0.1);
        if(hitcount < 2){
          renderer.sprite(vec3(0.87, 0.97, 0), vec4(1.0), 0.1);
          if(hitcount == 0){
            renderer.sprite(vec3(0.96, 0.97, 0), vec4(1.0), 0.1);
          }
        }
      }
    }
  }

  void draw() {
    
    
  if(gameover){
    elapsed += dt();
    renderer.beginShader("sprite");
    renderer.texture("image","gameover");
    renderer.push();
    renderer.scale(vec3(1.0, 0.5, 1.0));
    renderer.sprite(vec3(0.0, 0.0, 0.1), vec4(1.0), 1.0);
    renderer.pop();
    if(elapsed > 3){
      elapsed = 0;
      gameover = false;
    }
  }

  if (level == 4){
    won = true;
  }
  if(won){
    elapsed += dt();
    renderer.beginShader("sprite");
    renderer.texture("image","win");
    renderer.push();
    renderer.scale(vec3(1.0, 0.5, 1.0));
    renderer.sprite(vec3(0.0, 0.0, 0.1), vec4(1.0), 1.0);
    renderer.pop();
    if(elapsed > 3){
      elapsed = 0;
      won = false;
      restartGame();
      gameover = false;
    }
  }

  updateShooterPosition(dt());
  drawShooter();
  drawBullet();
  drawMeteor();
  updateMeteor();
  updateBullet();
  updateAliens(dt());
  drawAliens();
  checkBulletAlienCollision();
  drawLives();
  if(hitcount > 4 || checkShooterAlienCollision()){
    restartGame();
  }
  if(checkDone()){
    level++;
    numaliens = level *10;
    nextLevel();
    alienVel.y += -0.02;
  }
}

bool checkDone(){
      for(int i = 0; i < aliens.size(); i++){
        if(aliens[i].isalive){
          allDead = false;
          break;
        }else{
          allDead = true;
          
        }
      }
      if(allDead){
      }
      return allDead;
}

protected:
int numaliens = 10;
  vec3 alienVel = vec3(0.01, 0, 0);
  int level = 1;
  int hitcount = 0;
  bool allDead = false;
  Particle bullet;
  vec3 eyePos = vec3(0, 0, 3);
  vec3 lookPos = vec3(0, 0, 0);
  vec3 up = vec3(0, 1, 0);
  vec3 position = vec3(1, 0, 0);
  int frames = 0;
  bool shot = false;
  std::vector<Particle> mParticles;
  int index = 0;
  int score = 0;
  float timePassed = 0;
  float alienx = 0.01;
  int lives = 3;
  Particle meteor;
  float elapsed = 0;
  bool won = false;
};

int main(int argc, char** argv)
{
  Viewer viewer;
  viewer.run();
  return 0;
}