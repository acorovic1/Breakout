#include "Game.h"
#include "ResourceManager.h"
#include "SpriteRenderer.h"
#include "BallObject.h"
#include "ParticleGenerator.h"
#include "PostProcessor.h"

SpriteRenderer* Renderer;
GameObject* Player;
BallObject* Ball;
ParticleGenerator* Particles;
PostProcessor* Effects;

float ShakeTime = 0.0f;

const glm::vec2 INITIAL_BALL_VELOCITY(100.0f, -350.0f);
const float BALL_RADIUS = 12.5f;



Game::Game(unsigned int width, unsigned int height) :State(GAME_ACTIVE), Keys(), Width(width), Height(height)
{

};
Game::~Game() 
{
	delete Renderer;
	delete Player;
	delete Ball;
    delete Particles;
    delete Effects;
};

void Game::Init() 
{
 

	ResourceManager::LoadShader("Sprite.vert", "Sprite.frag", nullptr, "Sprite");
	ResourceManager::LoadShader("Particle.vert", "Particle.frag", nullptr, "Particle");
	ResourceManager::LoadShader("PostProcessing.vert", "PostProcessing.frag", nullptr, "Postprocessing");

	glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width),
		static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);

	ResourceManager::GetShader("Sprite").Use().SetInteger("image", 0);
	ResourceManager::GetShader("Sprite").Use().SetMatrix4("projection", projection);
    ResourceManager::GetShader("Particle").Use().SetInteger("sprite", 0);
    ResourceManager::GetShader("Particle").SetMatrix4("projection", projection);


	ResourceManager::LoadTexture("Textures/background.jpg", false, "background");
	ResourceManager::LoadTexture("Textures/awesomeface.png", true, "face");
	ResourceManager::LoadTexture("Textures/block.png", false, "block");
	ResourceManager::LoadTexture("Textures/block_solid.png", false, "block_solid");
	ResourceManager::LoadTexture("Textures/paddle.png", true, "paddle");
	ResourceManager::LoadTexture("Textures/particle.png", true, "particle");

	ResourceManager::LoadTexture("Textures/powerup_increase.png", true, "increase");
	ResourceManager::LoadTexture("Textures/powerup_confuse.png", true, "confuse");
	ResourceManager::LoadTexture("Textures/powerup_passthrough.png", true, "passthrough");
	ResourceManager::LoadTexture("Textures/powerup_speed.png", true, "speed");
	ResourceManager::LoadTexture("Textures/powerup_sticky.png", true, "sticky");
	ResourceManager::LoadTexture("Textures/powerup_chaos.png", true, "chaos");

    Renderer = new SpriteRenderer(ResourceManager::GetShader("Sprite"));
    Particles = new ParticleGenerator(ResourceManager::GetShader("Particle"), ResourceManager::GetTexture("particle"), 3000);
    Effects = new PostProcessor(ResourceManager::GetShader("Postprocessing"), this->Width, this->Height);


	GameLevel one; one.Load("one.lvl", this->Width, this->Height / 2);
	GameLevel two; two.Load("two.lvl", this->Width, this->Height / 2);
	GameLevel three; three.Load("three.lvl", this->Width, this->Height / 2);
	GameLevel four; four.Load("jhin.lvl", this->Width, this->Height / 2);
	this->Levels.push_back(one);
	this->Levels.push_back(two);
	this->Levels.push_back(three);
	this->Levels.push_back(four);
	this->Level = 0;

	glm::vec2 playerPos = glm::vec2(this->Width / 2.0f - PLAYER_SIZE.x / 2.0f, this->Height - PLAYER_SIZE.y);
	Player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::GetTexture("paddle"));

	glm::vec2 ballPos = playerPos + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -BALL_RADIUS * 2.0f);
	Ball = new BallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY, ResourceManager::GetTexture("face"));

    Effects->Chaos = Effects->Confuse = false;
    Ball->PassThrough = Ball->Sticky = false;
};
void Game::ProcessInput(float dt,GLFWwindow *window)
{
	if (this->State == GAME_ACTIVE )
	{
		float velocity = PLAYER_VELOCITY * dt;
		// move playerboard
		if (glfwGetKey(window,GLFW_KEY_A)==GLFW_PRESS)  
		{
			if (Player->Position.x >= 0.0f)
			{
				Player->Position.x -= velocity;
				if (Ball->Stuck)
					Ball->Position.x -= velocity;
			}
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		{
			if (Player->Position.x <= this->Width - Player->Size.x)
			{
				Player->Position.x += velocity;
				if (Ball->Stuck)
					Ball->Position.x += velocity;
			}
		}
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
			Ball->Stuck = false;
	}

    if (this->State == GAME_MENU)
    {
        if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS )
        {
            this->State = GAME_ACTIVE;
            
        }
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS  )
        {
            this->Level = (this->Level + 1) % 4;
           
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        {
            if (this->Level > 0)
                --this->Level;
            else
                this->Level = 3;
          
        }
    }
}

void Game:: Update(float dt) 
{
    Ball->Move(dt, this->Width);
    Particles->Update(dt, *Ball, 2, glm::vec2(Ball->Radius / 2.0f));
	this->DoCollisions();
    this->UpdatePowerUps(dt);

    if (ShakeTime > 0.0f)
    {
        ShakeTime -= dt;
        if (ShakeTime <= 0.0f)
            Effects->Shake = false;
    }

	if (Ball->Position.y >= this->Height) 
	{
		this->ResetLevel();
		this->ResetPlayer();
	}

    if (this->State == GAME_ACTIVE && this->Levels[this->Level].IsCompleted())
    {
        this->Level = (++this->Level) % 4;
        this->ResetLevel();
        this->ResetPlayer();
       // Effects->Chaos = true;
       // this->State = GAME_WIN;
    }
    
};

void Game:: Render() 
{
	if (this->State == GAME_ACTIVE)
	{
        Effects->BeginRender();
		
		Renderer->DrawSprite(ResourceManager::GetTexture("background"), glm::vec2(0.0f, 0.0f), glm::vec2(this->Width, this->Height), 0.0f);
		
		this->Levels[this->Level].Draw(*Renderer);
		
		Player->Draw(*Renderer);

        for (PowerUp& powerUp : this->PowerUps)
            if (!powerUp.Destroyed)
                powerUp.Draw(*Renderer);

        Particles->Draw();
		Ball->Draw(*Renderer);
       
        Effects->EndRender();
        Effects->Render(glfwGetTime());

       
	}
};

void Game::ResetLevel()
{
	if (this->Level == 0)
		this->Levels[0].Load("one.lvl", this->Width, this->Height / 2);
	else if (this->Level == 1)
		this->Levels[1].Load("two.lvl", this->Width, this->Height / 2);
	else if (this->Level == 2)
		this->Levels[2].Load("three.lvl", this->Width, this->Height / 2);
	else if (this->Level == 3)
		this->Levels[3].Load("jhin.lvl", this->Width, this->Height / 2);
}

void Game::ResetPlayer()
{
	// reset player/ball stats
	Player->Size = PLAYER_SIZE;
	Player->Position = glm::vec2(this->Width / 2.0f - PLAYER_SIZE.x / 2.0f, this->Height - PLAYER_SIZE.y);
	Ball->Reset(Player->Position + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -(BALL_RADIUS * 2.0f)), INITIAL_BALL_VELOCITY);

    Effects->Chaos = Effects->Confuse = false;
    Ball->PassThrough = Ball->Sticky = false;
    Player->Color = glm::vec3(1.0f);
    Ball->Color = glm::vec3(1.0f);

}

bool IsOtherPowerUpActive(std::vector<PowerUp>& powerUps, std::string type);

void Game::UpdatePowerUps(float dt)
{
    for (PowerUp& powerUp : this->PowerUps)
    {
        powerUp.Position += powerUp.Velocity * dt;
        if (powerUp.Activated)
        {
            powerUp.Duration -= dt;

            if (powerUp.Duration <= 0.0f)
            {
                // remove powerup from list (will later be removed)
                powerUp.Activated = false;
                // deactivate effects
                if (powerUp.Type == "sticky")
                {
                    if (!IsOtherPowerUpActive(this->PowerUps, "sticky"))
                    {	// only reset if no other PowerUp of type sticky is active
                        Ball->Sticky = false;
                        Player->Color = glm::vec3(1.0f);
                    }
                }
                else if (powerUp.Type == "passthrough")
                {
                    if (!IsOtherPowerUpActive(this->PowerUps, "passthrough"))
                    {	// only reset if no other PowerUp of type pass-through is active
                        Ball->PassThrough = false;
                        Ball->Color = glm::vec3(1.0f);
                    }
                }
                else if (powerUp.Type == "confuse")
                {
                    if (!IsOtherPowerUpActive(this->PowerUps, "confuse"))
                    {	// only reset if no other PowerUp of type confuse is active
                        Effects->Confuse = false;
                    }
                }
                else if (powerUp.Type == "chaos")
                {
                    if (!IsOtherPowerUpActive(this->PowerUps, "chaos"))
                    {	// only reset if no other PowerUp of type chaos is active
                        Effects->Chaos = false;
                    }
                }
            }
        }
    }
    this->PowerUps.erase(std::remove_if(this->PowerUps.begin(), this->PowerUps.end(),
        [](const PowerUp& powerUp) { return powerUp.Destroyed && !powerUp.Activated; }
    ), this->PowerUps.end());
}
bool ShouldSpawn(unsigned int chance)
{
    unsigned int random = rand() % chance;
    return random == 0;
}
void Game::SpawnPowerUps(GameObject& block)
{
    if (ShouldSpawn(75)) // 1 in 75 chance
        this->PowerUps.push_back(
            PowerUp("speed", glm::vec3(0.5f, 0.5f, 1.0f), 0.0f, block.Position, ResourceManager::GetTexture("speed")
            ));
    if (ShouldSpawn(75))
        this->PowerUps.push_back(
            PowerUp("sticky", glm::vec3(1.0f, 0.5f, 1.0f), 20.0f, block.Position, ResourceManager::GetTexture("sticky")
            ));
    if (ShouldSpawn(75))
        this->PowerUps.push_back(
            PowerUp("passthrough", glm::vec3(0.5f, 1.0f, 0.5f), 10.0f, block.Position, ResourceManager::GetTexture("passthrough")
            ));
    if (ShouldSpawn(75))
        this->PowerUps.push_back(
            PowerUp("increase", glm::vec3(1.0f, 0.6f, 0.4), 0.0f, block.Position, ResourceManager::GetTexture("increase")
            ));
    if (ShouldSpawn(15)) // negative powerups should spawn more often
        this->PowerUps.push_back(
            PowerUp("confuse", glm::vec3(1.0f, 0.3f, 0.3f), 15.0f, block.Position, ResourceManager::GetTexture("confuse")
            ));
    if (ShouldSpawn(15))
        this->PowerUps.push_back(
            PowerUp("chaos", glm::vec3(0.9f, 0.25f, 0.25f), 15.0f, block.Position, ResourceManager::GetTexture("chaos")
            ));
}

void ActivatePowerUp(PowerUp& powerUp)
{
    if (powerUp.Type == "speed")
    {
        Ball->Velocity *= 2;
    }
    else if (powerUp.Type == "sticky")
    {
        Ball->Sticky = true;
        Player->Color = glm::vec3(1.0f, 0.5f, 1.0f);
    }
    else if (powerUp.Type == "passthrough")
    {
        Ball->PassThrough = true;
        Ball->Color = glm::vec3(1.0f, 0.5f, 0.5f);
    }
    else if (powerUp.Type == "increase")
    {
        Player->Size.x += 50;
    }
    else if (powerUp.Type == "confuse")
    {
        if (!Effects->Chaos)
            Effects->Confuse = true; // only activate if chaos wasn't already active
    }
    else if (powerUp.Type == "chaos")
    {
        if (!Effects->Confuse)
            Effects->Chaos = true;
    }
}

bool IsOtherPowerUpActive(std::vector<PowerUp>& powerUps, std::string type)
{
    for (const PowerUp& powerUp : powerUps)
    {
        if (powerUp.Activated)
            if (powerUp.Type == type)
                return true;
    }
    return false;
}





bool CheckCollision(GameObject& one, GameObject& two);
Collision CheckCollision(BallObject& one, GameObject& two);
Direction VectorDirection(glm::vec2 closest);


void Game::DoCollisions()
{
    for (GameObject& box : this->Levels[this->Level].Bricks)
    {
        if (!box.Destroyed)
        {
            Collision collision = CheckCollision(*Ball, box);
            if (std::get<0>(collision)) // if collision is true
            {
                // destroy block if not solid
                if (!box.IsSolid)
                {
                    box.Destroyed = true;
                    this->SpawnPowerUps(box);
                }
                else
                {
                    ShakeTime += 0.05f;
                    Effects->Shake = true;
                }
                // collision resolution
                Direction dir = std::get<1>(collision);
                glm::vec2 diff_vector = std::get<2>(collision);
                if(!(Ball->PassThrough && !box.IsSolid))
                if (dir == LEFT || dir == RIGHT) // horizontal collision
                {
                    Ball->Velocity.x = -Ball->Velocity.x; // reverse horizontal velocity
                    // relocate
                    float penetration = Ball->Radius - std::abs(diff_vector.x);
                    if (dir == LEFT)
                        Ball->Position.x += penetration; // move ball to right
                    else
                        Ball->Position.x -= penetration; // move ball to left;
                }
                else // vertical collision
                {
                    Ball->Velocity.y = -Ball->Velocity.y; // reverse vertical velocity
                    // relocate
                    float penetration = Ball->Radius - std::abs(diff_vector.y);
                    if (dir == UP)
                        Ball->Position.y -= penetration; // move ball bback up
                    else
                        Ball->Position.y += penetration; // move ball back down
                }
            }
        }
        for (PowerUp& powerUp : this->PowerUps)
        {
            if (!powerUp.Destroyed)
            {
                if (powerUp.Position.y >= this->Height)
                    powerUp.Destroyed = true;
                if (CheckCollision(*Player, powerUp))
                {	// collided with player, now activate powerup
                    ActivatePowerUp(powerUp);
                    powerUp.Destroyed = true;
                    powerUp.Activated = true;
                }
            }
        }
    }
    // check collisions for player pad (unless stuck)
    Collision result = CheckCollision(*Ball, *Player);
    if (!Ball->Stuck && std::get<0>(result))
    {
        // check where it hit the board, and change velocity based on where it hit the board
        float centerBoard = Player->Position.x + Player->Size.x / 2.0f;
        float distance = (Ball->Position.x + Ball->Radius) - centerBoard;
        float percentage = distance / (Player->Size.x / 2.0f);
        // then move accordingly
        float strength = 2.0f;
        glm::vec2 oldVelocity = Ball->Velocity;
        Ball->Velocity.x = INITIAL_BALL_VELOCITY.x * percentage * strength;
        //Ball->Velocity.y = -Ball->Velocity.y;
        Ball->Velocity = glm::normalize(Ball->Velocity) * glm::length(oldVelocity); // keep speed consistent over both axes (multiply by length of old velocity, so total strength is not changed)
        // fix sticky paddle
        Ball->Velocity.y = -1.0f * abs(Ball->Velocity.y);

        Ball->Stuck = Ball->Sticky;
    }
}

bool CheckCollision(GameObject& one, GameObject& two) // AABB - AABB collision
{
    // collision x-axis?
    bool collisionX = one.Position.x + one.Size.x >= two.Position.x &&
        two.Position.x + two.Size.x >= one.Position.x;
    // collision y-axis?
    bool collisionY = one.Position.y + one.Size.y >= two.Position.y &&
        two.Position.y + two.Size.y >= one.Position.y;
    // collision only if on both axes
    return collisionX && collisionY;
}

Collision CheckCollision(BallObject& one, GameObject& two) // AABB - Circle collision
{
    // get center point circle first 
    glm::vec2 center(one.Position + one.Radius);
    // calculate AABB info (center, half-extents)
    glm::vec2 aabb_half_extents(two.Size.x / 2.0f, two.Size.y / 2.0f);
    glm::vec2 aabb_center(two.Position.x + aabb_half_extents.x, two.Position.y + aabb_half_extents.y);
    // get difference vector between both centers
    glm::vec2 difference = center - aabb_center;
    glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
    // now that we know the clamped values, add this to AABB_center and we get the value of box closest to circle
    glm::vec2 closest = aabb_center + clamped;
    // now retrieve vector between center circle and closest point AABB and check if length < radius
    difference = closest - center;

    if (glm::length(difference) < one.Radius) // not <= since in that case a collision also occurs when object one exactly touches object two, which they are at the end of each collision resolution stage.
        return std::make_tuple(true, VectorDirection(difference), difference);
    else
        return std::make_tuple(false, UP, glm::vec2(0.0f, 0.0f));
}

// calculates which direction a vector is facing (N,E,S or W)
Direction VectorDirection(glm::vec2 target)
{
    glm::vec2 compass[] = {
        glm::vec2(0.0f, 1.0f),	// up
        glm::vec2(1.0f, 0.0f),	// right
        glm::vec2(0.0f, -1.0f),	// down
        glm::vec2(-1.0f, 0.0f)	// left
    };
    float max = 0.0f;
    unsigned int best_match = -1;
    for (unsigned int i = 0; i < 4; i++)
    {
        float dot_product = glm::dot(glm::normalize(target), compass[i]);
        if (dot_product > max)
        {
            max = dot_product;
            best_match = i;
        }
    }
    return (Direction)best_match;
}



