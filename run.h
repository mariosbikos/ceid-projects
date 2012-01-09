#include <iostream>
#include <functional>
#include "iscreen.h"
#include "organism.h"
#include "Ocean.h"

class RunScreen : public IScreen
{
private:
    bool playing;
public:
    RunScreen (void);
    virtual int Run (sf::RenderWindow &App);
};

RunScreen::RunScreen(void){
    playing = false;
}

int Ocean::MAX_X;
int Ocean::MAX_Y;

int RunScreen::Run(sf::RenderWindow &App)
{
    sf::Event Event;
    bool Running = true;
    sf::Image runImage;
    sf::Image back;
    sf::Sprite runSprite;
    sf::Sprite backSprite;
    sf::Clock runner;
    sf::Vector2f MousePos;
    sf::Vector2f MousePosView;
    sf::Vector2i local;

    int zoom = 0;

    sf::View view;
    if(Ocean::worldIsBig){
        view.SetFromRect(sf::FloatRect(-5,-5,800,500));
        view.Zoom(0.35f);
        view.Move(722.0f,438.0f);
        if (!back.LoadFromFile("artwork/big.jpg")){
            std::cerr<<"Error loading background image"<<std::endl;
            return(-1);
        }
        backSprite.SetImage(back);
    }else{
        view.SetFromRect(sf::FloatRect(-5,-5,800,500));
        view.Zoom(0.81f);
        view.Move(87.f,49.f);
        if (!back.LoadFromFile("artwork/small.jpg")){
            std::cerr<<"Error loading background image"<<std::endl;
            return(-1);
        }
        backSprite.SetImage(back);
    }

    if (!runImage.LoadFromFile("artwork/run.png")){
        std::cerr<<"Error loading background image"<<std::endl;
        return(-1);
    }
    runSprite.SetImage(runImage);

    //App.Clear();

    float dt=1.f/5.f;          //change game rate
    float accumulator = 0.f;
    bool drawn= false;

    while (Running)
    {
        accumulator+=runner.GetElapsedTime();
        runner.Reset();

        while(accumulator>=dt){

            while(App.GetEvent(Event))
            {
                MousePos = App.ConvertCoords(App.GetInput().GetMouseX(), App.GetInput().GetMouseY());
                MousePosView = App.ConvertCoords(App.GetInput().GetMouseX(),App.GetInput().GetMouseY(),&view);
                if (Event.Type == sf::Event::Closed)
                {
                    return (-1);
                }
                if (Event.Type == sf::Event::KeyPressed && Event.Key.Code == sf::Key::Escape){
                    playing = true;
                    return(0);
                }
                if (Event.Type == sf::Event::MouseButtonPressed && Event.MouseButton.Button == sf::Mouse::Left){
                    local = Helper::getLocalCoords(MousePosView.x,MousePosView.y);
                    //debugging
                    std::cout << local.x << " " << local.y << std::endl;
                }
                if (Event.Type == sf::Event::KeyPressed && Event.Key.Code == sf::Key::Space) {
                    Ocean::pollute(rand()%5 + 1, rand()%Ocean::MAX_X, rand()%Ocean::MAX_Y, rand()%8 + 3);
                }
                if (Event.Type == sf::Event::MouseButtonPressed && Event.MouseButton.Button == sf::Mouse::Left){
                    if(MousePos.x > 826 && MousePos.x < 1015 && MousePos.y > 558 && MousePos.y < 594){
                        IScreen::logChoice = !IScreen::logChoice;
                    }
                }
                /////////////////////////////////////////////////////
                if (Event.Type == sf::Event::KeyPressed && Event.Key.Code == sf::Key::W){
                    if(zoom < 2){
                        view.Zoom(1.25f);
                        zoom++;
                    }
                }
                if (Event.Type == sf::Event::KeyPressed && Event.Key.Code == sf::Key::S){
                    if(zoom >= 0){//temporary for debugging
                        view.Zoom(0.5f);
                        zoom--;
                    }
                }

                if (Event.Type == sf::Event::KeyPressed && Event.Key.Code == sf::Key::Up){
                    view.Move(0, 10.0f);
                }
                if (Event.Type == sf::Event::KeyPressed && Event.Key.Code == sf::Key::Down){
                    view.Move(0, -10.0f);
                }
                if (Event.Type == sf::Event::KeyPressed && Event.Key.Code == sf::Key::Left){
                    view.Move(10.0f, 0);
                }
                if (Event.Type == sf::Event::KeyPressed && Event.Key.Code == sf::Key::Right){
                    view.Move(-10.0f, 0);
                }

                ////////////////////////////////////////////////////
            }

            Ocean::update();

            accumulator-=dt;
            drawn=false;
            //b++;
            //std::cout<<"update= "<<b<<std::endl;
        }

        if(drawn){
            sf::Sleep(0.01);
        }
        else{
            App.SetView(view);

            App.Draw(backSprite);
            std::map<int, Organism*>::iterator it;
            for(it = Ocean::fishMap.begin();it != Ocean::fishMap.end(); it++){
                App.Draw(it->second->sprite);
            }

            Pollution::bind(&App);
            std::for_each(Ocean::pollution.begin(), Ocean::pollution.end(), std::mem_fun(&Pollution::draw));
            Pollution::bind(0);

            App.SetView(App.GetDefaultView());
            App.Draw(runSprite);

            //Draw stats box
            Ocean::drawStats(&App, IScreen::logChoice, 0);

            App.Display();
            App.Clear();

            drawn=true;
            //a++;

            //debug for placing view
            //std::cout<<view.GetRect().Right<<","<<view.GetRect().Top<<" "<<view.GetRect().Left<<","<<view.GetRect().Bottom<<std::endl;
            //std::cout <<"render= "<<a<<std::endl<<std::endl;
        }


    }

    return(-1);
}
