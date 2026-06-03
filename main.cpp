#define SFML_ENABLED

#include "math.h"
#include "surface.h"
#include "visualization.h"

#ifdef SFML_ENABLED
#include <SFML/Graphics.hpp>
#endif

using namespace std;

int main()
{
    Scene scene;

    // Giant refractive crystal
    scene.add(
        new sphericallens(
            Vector3D(8,0,0),
            Vector3D(1,0,0),
            6.0,
            "Air",
            "Diamond"
        )
    );

    // Left mirror
    scene.add(
        new flatMirror(
            Vector3D(20,-8,0),
            Vector3D(-1,1,0),
            "Blood"
        )
    );

    // Right mirror
    scene.add(
        new flatMirror(
            Vector3D(20,8,0),
            Vector3D(-1,-1,0),
            "Blood"
        )
    );

#ifdef SFML_ENABLED

    SFMLVisualizer visualizer(1600,900,25);

    visualizer.setScene(&scene);

    // MANY rays
    for(int i=-120;i<=120;i++)
    {
        float angle = i * 0.0025f;

        visualizer.addRay(
            Ray(
                Vector3D(-30,0,0),
                Vector3D(1,angle,0)
            ),

            sf::Color(
                50,
                255 - abs(i),
                255
            )
        );
    }

    visualizer.traceAllRays();

    sf::Clock clock;

    while(visualizer.isOpen())
    {
        float dt = clock.restart().asSeconds();

        visualizer.handleEvents();

        visualizer.update(dt);

        visualizer.draw();
    }

#endif
}