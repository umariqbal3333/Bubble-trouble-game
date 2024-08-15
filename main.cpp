#include <simplecpp>
#include "shooter.h"

/* Simulation Vars */
const double STEP_TIME = 0.01;

/* Game Vars */
const int PLAY_Y_HEIGHT = 450;
const int LEFT_MARGIN = 70;
const int TOP_MARGIN = 20;
const int BOTTOM_MARGIN = (PLAY_Y_HEIGHT+TOP_MARGIN);
const int TIME_LIMIT = 60;                                                                                                                              // Modification (Added)
const int MAX_HEALTH = 3;                                                                                                                               // Modification (Added)

// "bubble.h" header file is included after declaring "PLAY_Y_HEIGHT" variable as this variable is required in the header file
#include "bubble.h"                                                                                                                     // Modification (bubble.h included later)


// Returns the string which is to be displayed on the screen
string string_displayed(string var_name, int var)                                                                                                       // Modification (Added)
{
    string s1 = var_name;
    string s2;
    stringstream stream;
    stream << var;
    stream >> s2;

    return s1.append(s2);
}


void move_bullets(vector<Bullet> &bullets){
    // move all bullets
    for(unsigned int i=0; i<bullets.size(); i++){
        if(!bullets[i].nextStep(STEP_TIME)){
            bullets.erase(bullets.begin()+i);
        }
    }
}

void move_bubbles(vector<Bubble> &bubbles){
    // move all bubbles
    for (unsigned int i=0; i < bubbles.size(); i++)
    {
        bubbles[i].nextStep(STEP_TIME);
    }
}


// Takes in an integer as a parameter so as to change the number of bubbles and their properties with changing level
vector<Bubble> create_bubbles(int level)                                                                                            // Modification (Takes level as an argument)
{
    // create initial bubbles in the game
    vector<Bubble> bubbles;

    if(level == 1)
    {
        bubbles.push_back(Bubble(WINDOW_X/2.0, BUBBLE_START_Y, BUBBLE_DEFAULT_RADIUS, -BUBBLE_DEFAULT_VX, 0, COLOR(0,100,100)));
        bubbles.push_back(Bubble(WINDOW_X/4.0, BUBBLE_START_Y, BUBBLE_DEFAULT_RADIUS, BUBBLE_DEFAULT_VX, 0, COLOR(0,100,100)));
    }
    else if(level == 2)
    {
        bubbles.push_back(Bubble(WINDOW_X/1.5, BUBBLE_START_Y, 2*BUBBLE_DEFAULT_RADIUS, -2*BUBBLE_DEFAULT_VX, 0, COLOR(85,50,180)));
        bubbles.push_back(Bubble(WINDOW_X/3.0, BUBBLE_START_Y, 2*BUBBLE_DEFAULT_RADIUS, BUBBLE_DEFAULT_VX, 0, COLOR(85,50,180)));
        bubbles.push_back(Bubble(WINDOW_X/6.0, BUBBLE_START_Y, BUBBLE_DEFAULT_RADIUS, -BUBBLE_DEFAULT_VX, 0, COLOR(85,50,180)));
        bubbles.push_back(Bubble(WINDOW_X/1.2, BUBBLE_START_Y, BUBBLE_DEFAULT_RADIUS, 2*BUBBLE_DEFAULT_VX, 0, COLOR(85,50,180)));
    }
    else if(level == 3)
    {
        bubbles.push_back(Bubble(WINDOW_X/1.4, BUBBLE_START_Y, 4*BUBBLE_DEFAULT_RADIUS, -3*BUBBLE_DEFAULT_VX, 0, COLOR(200, 0, 50)));
        bubbles.push_back(Bubble(WINDOW_X/3.5, BUBBLE_START_Y, 4*BUBBLE_DEFAULT_RADIUS, 3*BUBBLE_DEFAULT_VX, 0, COLOR(200, 0, 50)));
        bubbles.push_back(Bubble(WINDOW_X/2, BUBBLE_START_Y, 4*BUBBLE_DEFAULT_RADIUS, 2*BUBBLE_DEFAULT_VX, 0, COLOR(200, 0, 50)));
    }

    return bubbles;
}


void bullet_hits_bubble(vector<Bubble> &bubbles, vector<Bullet> &bullets, int &score, vector<bool> &did_bubble_hit)                                     // Modification (Added)
{
    // check if any bullet hit any of the bubbles and if yes, then remove the bullet, disintegrate or destroy the bubble and increase the score
    // Also update did_bubble_hit
    for (unsigned int i=0; i < bubbles.size(); i++)
    {
        for (unsigned int j=0; j < bullets.size(); j++)
        {
            if( (bubbles[i].get_center_x() - bullets[j].get_center_x())*(bubbles[i].get_center_x() - bullets[j].get_center_x())
                + (bubbles[i].get_center_y() - bullets[j].get_center_y())*(bubbles[i].get_center_y() - bullets[j].get_center_y())
                <= (bullets[j].get_width()/2.0 + bubbles[i].get_radius())*(bullets[j].get_width()/2.0 + bubbles[i].get_radius()) )
            {
                bullets.erase(bullets.begin()+j);

                if(bubbles[i].get_radius() == BUBBLE_DEFAULT_RADIUS)
                {
                    // Erasing the bubble and not disintgrating it, since its radius is equal to BUBBLE_DEFAULT_RADIUS
                    bubbles.erase(bubbles.begin()+i);
                    did_bubble_hit.erase(did_bubble_hit.begin()+i);
                }
                else
                {
                    // Inserting 2 bubbles with half of initial radius and erasing the initial bubble
                    bubbles.insert(bubbles.begin()+i, Bubble(bubbles[i].get_center_x(), bubbles[i].get_center_y(), bubbles[i].get_radius()/2.0,
                                                             bubbles[i].get_vx(), bubbles[i].get_vy(), bubbles[i].get_color()));
                    did_bubble_hit.insert(did_bubble_hit.begin()+i, false);

                    bubbles.insert(bubbles.begin()+i+1, Bubble(bubbles[i+1].get_center_x(), bubbles[i+1].get_center_y(), bubbles[i+1].get_radius()/2.0,
                                                              -bubbles[i+1].get_vx(), bubbles[i+1].get_vy(), bubbles[i+1].get_color()));
                    did_bubble_hit.insert(did_bubble_hit.begin()+i+1, false);

                    bubbles.erase(bubbles.begin()+i+2);
                    did_bubble_hit.erase(did_bubble_hit.begin()+i+2);
                }
                score++;
            }
        }
    }
}


void bubble_hits_shooter(vector<Bubble> &bubbles, Shooter shooter, int &health, vector<bool> &did_bubble_hit, bool &shooter_hit)                        // Modification (Added)
{
    // Check if any of the bubbles hit the shooter, and if yes, then decrease health by the number of bubbles that hit the shooter

    // Tells whether the shooter is overlapped by any bubble. This value is then to be assigned to shooter_hit
    bool shooter_overlapped = false;

    // Sets the values of did_bubble_hit for each of the bubbles
    for (unsigned int i=0; i < bubbles.size(); i++)
    {
        // Tells whether that specific bubble is overlapping the shooter or not
        bool bubble_overlaps_shooter = ((bubbles[i].get_center_x() - shooter.get_head_center_x())*(bubbles[i].get_center_x() - shooter.get_head_center_x())
                                        + (bubbles[i].get_center_y() - shooter.get_head_center_y())*(bubbles[i].get_center_y() - shooter.get_head_center_y())
                                        <= (shooter.get_head_radius() + bubbles[i].get_radius())*(shooter.get_head_radius() + bubbles[i].get_radius())
                                        ||
                                      (abs(bubbles[i].get_center_x() - shooter.get_body_center_x()) <= bubbles[i].get_radius() + shooter.get_body_width()/2.0
                                       && bubbles[i].get_center_y() >= shooter.get_body_center_y() - shooter.get_body_height()/2.0));

        if(bubble_overlaps_shooter)
        {
            shooter_overlapped = true;
            if(!did_bubble_hit[i])  // Since that particular bubble hit the shooter, and did_bubble_hit corresponding to it is false, means that the bubble has just now hit
            {
                did_bubble_hit[i] = true;
                health--;
                if(health == 0)
                    break;
            }
        }
        else if(did_bubble_hit[i] == true)  // Since that bubble was earlier overlapping but now isn't, means that it just now lost contact with the shooter
            did_bubble_hit[i] = false;
    }
    shooter_hit = shooter_overlapped;
}


int main()
{
    initCanvas("Bubble Trouble", WINDOW_X, WINDOW_Y);

    int score = 0;                                                                                                                                      // Modification (Added)
    int score_initially = score;

    for(unsigned int level = 1; level <= 3; level++)                                                                                                    // Modification (Added)
    {
        // Displaying level before starting that level
        string l_before_start = string_displayed("Level: ", level);                                                                                     // Modification (Added)
        {
            Text display(WINDOW_X/2.0, WINDOW_Y/2.0, l_before_start);
            display.setColor(COLOR(0, 0, 255));
            wait(5);
        }


        // Display level during the game
        string l_while_playing = l_before_start.append("/3");                                                                                           // Modification (Added)
        Text display_level(WINDOW_X/2.0, BOTTOM_MARGIN, l_while_playing);


        // Display health
        int health = MAX_HEALTH;                                                                                                                        // Modification (Added)
        int health_initially = health;
        string s_health1 = string_displayed("Health: ", health).append( string_displayed("/", MAX_HEALTH) );
        Text display_health(WINDOW_X - LEFT_MARGIN, TOP_MARGIN/2.0, s_health1);


        Line b1(0, PLAY_Y_HEIGHT, WINDOW_X, PLAY_Y_HEIGHT);
        b1.setColor(COLOR(0, 0, 255));


        string msg_cmd("Cmd: _");
        Text charPressed(LEFT_MARGIN, BOTTOM_MARGIN, msg_cmd);


        // Display score
        string s_score = string_displayed("Score: ", score);                                                                                            // Modification (Added)
        Text display_score(WINDOW_X - LEFT_MARGIN, BOTTOM_MARGIN, s_score);


        // Display time
        double time = TIME_LIMIT;
        string s_time = string_displayed("Time: ", time);                                                                                               // Modification (Added)
        Text display_time(LEFT_MARGIN, TOP_MARGIN/2.0, s_time);

        double time_initially = time;


        // Initialize the shooter
        Shooter shooter(SHOOTER_START_X, SHOOTER_START_Y, SHOOTER_VX);

        // Store the original colour of shooter as later we'll need to change its colour back to original, after getting hit by a bubble
        Color original_color = shooter.get_shooter_color();                                                                                             // Modification (Added)


        // Initialize the bubbles
        vector<Bubble> bubbles = create_bubbles(level);                                                                             // Modification (Taking level as an argument)


        // Initialize the bullets (empty)
        vector<Bullet> bullets;

        XEvent event;

        // Tells whether the shooter is still hit/being overlapped by any bubble
        bool shooter_hit = false;                                                                                                                       // Modification (Added)

        // This boolean will be used later to ensure that shooter colour is reset only when bubble leaves the shooter
        bool a = false;                                                                                                                                 // Modification (Added)

        // Tells for each bubble, whether it is currently in contact or overlapping the shooter (initially all are false)
        vector<bool> did_bubble_hit (bubbles.size(), false);                                                                                            // Modification (Added)


        // Main game loop
        while (true)
        {
            bool pendingEvent = checkEvent(event);
            if (pendingEvent)
            {
                // Get the key pressed
                char c = charFromEvent(event);
                msg_cmd[msg_cmd.length() - 1] = c;
                charPressed.setMessage(msg_cmd);

                // Update the shooter
                if(c == 'a')
                    shooter.move(STEP_TIME, true);
                else if(c == 'd')
                    shooter.move(STEP_TIME, false);
                else if(c == 'w')
                    bullets.push_back(shooter.shoot());
                else if(c == 'q')
                    return 0;
            }

            // Update the bubbles
            move_bubbles(bubbles);


            // Update the bullets
            move_bullets(bullets);


            // Remove/disintegrate bubbles and bullets which have collided and also update score
            score_initially = score;                                                                                                                    // Modification (Added)
            bullet_hits_bubble(bubbles, bullets, score, did_bubble_hit);
            if(score != score_initially)
            {
                s_score = ( string_displayed("Score: ", score) );
                display_score.setMessage(s_score);
            }


            // In case all the bubbles are destroyed, then break from the while loop
            if(bubbles.empty())                                                                                                                         // Modification (Added)
                break;


            // Update health in case the shooter is hit
            // While the bubble is still overlapping, shooter_hit will still be true in those iterations
            // So to avoid updating health display unnecessarily, change in health is also made mandatory
            health_initially = health;
            bubble_hits_shooter(bubbles, shooter, health, did_bubble_hit, shooter_hit);
            if(shooter_hit && health != health_initially)                                                                                               // Modification (Added)
            {
                s_health1 = string_displayed("Health: ", health).append( string_displayed("/", MAX_HEALTH) );
                display_health.setMessage(s_health1);
                a = true;

                shooter.set_shooter_color(COLOR(255, 0, 0));
            }

            // This statement will be executed only when "a" is "true" and "shooter_hit" is "false", ie, only at the moment when bubbles stop overlapping the shooter
            // It resets the color of shooter to original and sets "a" to "false"
            if(a && !shooter_hit){                                                                                                                      // Modification (Added)
                shooter.set_shooter_color(original_color);
                a = false;
            }


            // Game Over if the shooter is out of health
            if(health == 0)                                                                                                                             // Modification (Added)
            {
                Text loose(WINDOW_X/2.0, WINDOW_Y/2.0, "Game Over");
                loose.setColor(COLOR(255, 0, 0));
                getClick();
                return 0;
            }

            // Update time
            time -= 0.035;                                                                                                                              // Modification (Added)
            // If time becomes less than an integer without becoming equal to it, then at that instant time display should be updated
            if(time <= int(time_initially) && time_initially >= int(time_initially))
            {
                s_time = string_displayed( "Time: ", int(time_initially) );
                display_time.setMessage(s_time);

                if(int(time_initially) == 0)
                {
                    Text time_out(WINDOW_X/2.0, WINDOW_Y/2.0, "Game Over");
                    time_out.setColor(COLOR(255, 0, 0));
                    getClick();
                    return 0;
                }
            }
            time_initially = time;


            wait(STEP_TIME);
        }

        // At level = 3, breaking from above while loop means that all the bubbles have been destroyed
        if(level == 3)                                                                                                                                  // Modification (Added)
        {
            Text display(WINDOW_X/2.0, WINDOW_Y/2.0, "Congratulations!!");
            display.setColor(COLOR(0, 255, 0));
            getClick();
            return 0;
        }
    }
}
