#pragma once
#include "math.h"
#include "surface.h"
#include <vector>
#include <cmath>
#include <algorithm>
#ifdef SFML_ENABLED
#include <SFML/Graphics.hpp>
#endif


class RayPath {

public:

    struct Segment {

        Vector3D startPoint;

        Vector3D direction;

        double length;

        Segment(
            const Vector3D& s,
            const Vector3D& d,
            double l
        )
            :
            startPoint(s),
            direction(d.normalized()),
            length(l)
        {}
    };

private:

    std::vector<Segment> m_segments;

public:

    void addSegment(
        const Vector3D& start,
        const Vector3D& dir,
        double len
    ) {
        m_segments.push_back(
            Segment(start, dir, len)
        );
    }

    const std::vector<Segment>& getSegments() const {

        return m_segments;
    }
};

class RayTracer {

private:

    int m_bounceLimit;

public:

    RayTracer(int maxBounces = 20)
        :
        m_bounceLimit(maxBounces)
    {}

    RayPath trace(
        Ray ray,
        Scene& scene
    ) const {

        RayPath path;

        for (int bounce = 0;
             bounce < m_bounceLimit;
             bounce++)
        {
            HitRecord closestRec;

            surface* hitSurface = nullptr;

            if (!scene.hitClosest(ray, closestRec)) {

                path.addSegment(
                    ray.origin(),
                    ray.direction(),
                    1000.0
                );

                break;
            }

            const auto& surfaces =
                scene.getSurfaces();

            for (size_t i = 0;
                 i < surfaces.size();
                 i++)
            {
                HitRecord temp;

                if (surfaces[i]->intersect(ray, temp)) {

                    if (std::abs(temp.t - closestRec.t) < 1e-5) {

                        hitSurface = surfaces[i];

                        break;
                    }
                }
            }

            if (!hitSurface) {

                path.addSegment(
                    ray.origin(),
                    ray.direction(),
                    1000.0
                );

                break;
            }

            path.addSegment(
                ray.origin(),
                ray.direction(),
                closestRec.t
            );

            Ray outRay;

            hitSurface->interact(
                ray,
                closestRec,
                outRay
            );

            ray = outRay;
        }

        return path;
    }
};

#ifdef SFML_ENABLED

class SFMLVisualizer {

public:

    enum RenderMode {

        WIREFRAME,

        RIGID
    };

private:

    struct RayData {

        Ray ray;

        sf::Color color;

        RayPath path;

        bool traced;
    };

    sf::RenderWindow m_window;

    sf::View m_view;

    Scene* m_scene;

    RayTracer m_tracer;

    std::vector<RayData> m_rays;

    RenderMode m_renderMode;

    bool m_animating;

    float m_animTime;

    float m_animSpeed;

    float m_pauseTimer;

    float m_animPause;

public:

    SFMLVisualizer(
        int width = 1400,
        int height = 900,
        int maxBounces = 20
    )

        :

        m_window(
            sf::VideoMode(width, height),
            "OPTICS SIMULATION"
        ),

        m_tracer(maxBounces),

        m_scene(nullptr),

        m_renderMode(RIGID),

        m_animating(true),

        m_animTime(0.0f),

        m_animSpeed(20.0f),

        m_pauseTimer(0.0f),

        m_animPause(.8f)
    {
        m_view.reset(
            sf::FloatRect(
                -25,
                -15,
                70,
                35
            )
        );

        m_window.setView(m_view);

        m_window.setFramerateLimit(60);
    }


    void setScene(Scene* scene) {

        m_scene = scene;
    }


    void addRay(
        const Ray& ray,
        sf::Color color = sf::Color::Yellow
    ) {
        RayData rd;

        rd.ray = ray;

        rd.color = color;

        rd.traced = false;

        m_rays.push_back(rd);
    }


    void traceAllRays() {

        for (size_t i = 0;
             i < m_rays.size();
             i++)
        {
            m_rays[i].path =
                m_tracer.trace(
                    m_rays[i].ray,
                    *m_scene
                );

            m_rays[i].traced = true;
        }
    }


    bool isOpen() const {

        return m_window.isOpen();
    }

    void update(float dt) {

        if (!m_animating) {

            m_pauseTimer -= dt;

            if (m_pauseTimer <= 0.0f) {

                m_animTime  = 0.0f;
                m_animating = true;
            }

            return;
        }

        m_animTime += m_animSpeed * dt;
        float cycleEnd =
            static_cast<float>(m_rays.size()) * 8.0f + 300.0f;

        if (m_animTime > cycleEnd) {

            m_animating  = false;
            m_pauseTimer = m_animPause;   
        }
    }


    void handleEvents() {

        sf::Event event;

        while (m_window.pollEvent(event)) {

            if (event.type ==
                sf::Event::Closed)
            {
                m_window.close();
            }

            if (event.type ==
                sf::Event::KeyPressed)
            {
                switch (event.key.code) {

                case sf::Keyboard::Escape:
                    m_window.close();
                    break;

                case sf::Keyboard::Num1:
                    m_renderMode = WIREFRAME;
                    break;

                case sf::Keyboard::Num2:
                    m_renderMode = RIGID;
                    break;

                case sf::Keyboard::Left:
                    m_view.move(-2, 0);
                    break;

                case sf::Keyboard::Right:
                    m_view.move(2, 0);
                    break;

                case sf::Keyboard::Up:
                    m_view.move(0, -2);
                    break;

                case sf::Keyboard::Down:
                    m_view.move(0, 2);
                    break;

                case sf::Keyboard::Z:
                    m_view.zoom(0.9f);
                    break;

                case sf::Keyboard::X:
                    m_view.zoom(1.1f);
                    break;

                default:
                    break;
                }
            }
        }

        m_window.setView(m_view);
    }


    void draw() {

        m_window.clear(
            sf::Color(8, 8, 20)
        );
        sf::RectangleShape fade;
        fade.setSize(sf::Vector2f(1e6f, 1e6f));
        fade.setPosition(-5e5f, -5e5f);
        fade.setFillColor(sf::Color(8, 8, 20, 30));  
        m_window.draw(fade);

        drawGrid();

        drawSurfaces();

        drawRays();

        m_window.display();
    }

private:


    void drawGrid() {

        sf::VertexArray grid(sf::Lines);

        for (float x = -100; x <= 100; x += 1) {

            sf::Color c = (x == 0.0f)
                ? sf::Color(70, 90, 160, 180)
                : sf::Color(35, 35, 50);

            grid.append(
                sf::Vertex(sf::Vector2f(x, -100), c)
            );

            grid.append(
                sf::Vertex(sf::Vector2f(x,  100), c)
            );
        }

        for (float y = -100; y <= 100; y += 1) {

            sf::Color c = (y == 0.0f)
                ? sf::Color(70, 90, 160, 180)
                : sf::Color(35, 35, 50);

            grid.append(
                sf::Vertex(sf::Vector2f(-100, y), c)
            );

            grid.append(
                sf::Vertex(sf::Vector2f( 100, y), c)
            );
        }

        m_window.draw(grid);
    }


    void drawSurfaces() {

        if (!m_scene)
            return;

        const auto& surfaces =
            m_scene->getSurfaces();

        for (size_t i = 0;
             i < surfaces.size();
             i++)
        {
            surface* s = surfaces[i];

            if (flatMirror* fm =
                dynamic_cast<flatMirror*>(s))
            {
                Vector3D p =
                    fm->getPoint();

                Vector3D n =
                    fm->getNormal();

                Vector3D along(
                    -n.y,
                    n.x,
                    0
                );

                float len = 8.0f;

                if (m_renderMode == WIREFRAME)
                {
                    sf::VertexArray line(
                        sf::Lines,
                        2
                    );

                    line[0] =
                        sf::Vertex(
                            sf::Vector2f(
                                (float)(p.x - along.x * len),
                                (float)(p.y - along.y * len)
                            ),
                            sf::Color::White
                        );

                    line[1] =
                        sf::Vertex(
                            sf::Vector2f(
                                (float)(p.x + along.x * len),
                                (float)(p.y + along.y * len)
                            ),
                            sf::Color::White
                        );

                    m_window.draw(line);
                }
                else {

                    float thickness = 0.35f;

                    sf::ConvexShape body;

                    body.setPointCount(4);

                    body.setPoint(0,
                        sf::Vector2f(
                            (float)(p.x - along.x*len - n.x*thickness),
                            (float)(p.y - along.y*len - n.y*thickness)
                        )
                    );

                    body.setPoint(1,
                        sf::Vector2f(
                            (float)(p.x + along.x*len - n.x*thickness),
                            (float)(p.y + along.y*len - n.y*thickness)
                        )
                    );

                    body.setPoint(2,
                        sf::Vector2f(
                            (float)(p.x + along.x*len + n.x*thickness),
                            (float)(p.y + along.y*len + n.y*thickness)
                        )
                    );

                    body.setPoint(3,
                        sf::Vector2f(
                            (float)(p.x - along.x*len + n.x*thickness),
                            (float)(p.y - along.y*len + n.y*thickness)
                        )
                    );

                    body.setFillColor(
                        sf::Color(70,70,85)
                    );

                    body.setOutlineThickness(0.08f);

                    body.setOutlineColor(
                        sf::Color(220,220,255)
                    );

                    m_window.draw(body);
                }
            }


            else if (convexmirror* cvm =
                dynamic_cast<convexmirror*>(s))
            {
                drawCurvedMirror(
                    cvm->getVertex(),
                    cvm->getAxis(),
                    -cvm->getRadius(),
                    sf::Color(180,150,150)
                );
            }


            else if (concavemirror* cm =
                dynamic_cast<concavemirror*>(s))
            {
                drawCurvedMirror(
                    cm->getVertex(),
                    cm->getAxis(),
                    cm->getRadius(),
                    sf::Color(180,180,255)
                );

                Vector3D v  = cm->getVertex();
                Vector3D ax = cm->getAxis();
                double   f  = cm->focalpoint();   

                float fx = (float)(v.x + ax.x * f);
                float fy = (float)(v.y + ax.y * f);

                float cs = 0.45f;

                sf::VertexArray cross(sf::Lines, 4);

                cross[0] = sf::Vertex(
                    sf::Vector2f(fx - cs, fy),
                    sf::Color(255, 215, 50, 200)
                );
                cross[1] = sf::Vertex(
                    sf::Vector2f(fx + cs, fy),
                    sf::Color(255, 215, 50, 200)
                );
                cross[2] = sf::Vertex(
                    sf::Vector2f(fx, fy - cs),
                    sf::Color(255, 215, 50, 200)
                );
                cross[3] = sf::Vertex(
                    sf::Vector2f(fx, fy + cs),
                    sf::Color(255, 215, 50, 200)
                );

                m_window.draw(cross);

                // Dot
                sf::CircleShape dot(0.20f);
                dot.setOrigin(0.20f, 0.20f);
                dot.setPosition(fx, fy);
                dot.setFillColor(sf::Color(255, 215, 50, 220));
                m_window.draw(dot);
            }


            else if (flatRefractor* fr =
                dynamic_cast<flatRefractor*>(s))
            {
                Vector3D p =
                    fr->getPoint();

                Vector3D n =
                    fr->getNormal();

                Vector3D along(
                    -n.y,
                    n.x,
                    0
                );

                float len = 10.0f;

                float thickness = 0.5f;

                sf::ConvexShape glass;

                glass.setPointCount(4);

                glass.setPoint(0,
                    sf::Vector2f(
                        (float)(p.x - along.x*len - n.x*thickness),
                        (float)(p.y - along.y*len - n.y*thickness)
                    )
                );

                glass.setPoint(1,
                    sf::Vector2f(
                        (float)(p.x + along.x*len - n.x*thickness),
                        (float)(p.y + along.y*len - n.y*thickness)
                    )
                );

                glass.setPoint(2,
                    sf::Vector2f(
                        (float)(p.x + along.x*len + n.x*thickness),
                        (float)(p.y + along.y*len + n.y*thickness)
                    )
                );

                glass.setPoint(3,
                    sf::Vector2f(
                        (float)(p.x - along.x*len + n.x*thickness),
                        (float)(p.y - along.y*len + n.y*thickness)
                    )
                );

                glass.setFillColor(
                    sf::Color(
                        100,
                        220,
                        255,
                        80
                    )
                );

                glass.setOutlineThickness(0.05f);

                glass.setOutlineColor(
                    sf::Color(
                        180,
                        255,
                        255
                    )
                );

                m_window.draw(glass);
            }

            else if (sphericallens* sl =
                dynamic_cast<sphericallens*>(s))
            {
                Vector3D cen =
                    sl->getCenter();

                double R =
                    sl->getRadius();

                sf::CircleShape lens(
                    (float)R
                );

                lens.setOrigin(
                    (float)R,
                    (float)R
                );

                lens.setPosition(
                    (float)cen.x,
                    (float)cen.y
                );

                lens.setFillColor(
                    sf::Color(
                        100,
                        255,
                        180,
                        70
                    )
                );

                lens.setOutlineThickness(0.08f);

                lens.setOutlineColor(
                    sf::Color(
                        180,
                        255,
                        220
                    )
                );

                m_window.draw(lens);
            }
        }
    }


    void drawCurvedMirror(
        const Vector3D& vertex,
        const Vector3D& axis,
        double radius,
        sf::Color color
    ) {
        Vector3D cen =
            vertex + axis * radius;

        int steps = 80;

        float halfArc = 0.8f;

        double base =
            atan2(-axis.y, -axis.x);

        if (radius < 0)
            base += 3.1415926;

        if (m_renderMode == WIREFRAME)
        {
            sf::VertexArray arc(
                sf::LineStrip,
                steps + 1
            );

            for (int j = 0; j <= steps; j++)
            {
                double a =
                    base - halfArc
                    +
                    (
                        2.0 * halfArc * j
                        / steps
                    );

                arc[j] =
                    sf::Vertex(
                        sf::Vector2f(
                            (float)(
                                cen.x
                                +
                                std::abs(radius)
                                * cos(a)
                            ),
                            (float)(
                                cen.y
                                +
                                std::abs(radius)
                                * sin(a)
                            )
                        ),
                        color
                    );
            }

            m_window.draw(arc);
        }
        else {

            float thickness = 0.5f;

            sf::ConvexShape body;

            body.setPointCount(
                (steps + 1) * 2
            );

            for (int j = 0; j <= steps; j++)
            {
                double a =
                    base - halfArc
                    +
                    (
                        2.0 * halfArc * j
                        / steps
                    );

                body.setPoint(
                    j,
                    sf::Vector2f(
                        (float)(
                            cen.x
                            +
                            (
                                std::abs(radius)
                                + thickness
                            )
                            *
                            cos(a)
                        ),

                        (float)(
                            cen.y
                            +
                            (
                                std::abs(radius)
                                + thickness
                            )
                            *
                            sin(a)
                        )
                    )
                );
            }

            for (int j = 0; j <= steps; j++)
            {
                double a =
                    base + halfArc
                    -
                    (
                        2.0 * halfArc * j
                        / steps
                    );

                body.setPoint(
                    j + steps + 1,
                    sf::Vector2f(
                        (float)(
                            cen.x
                            +
                            (
                                std::abs(radius)
                                - thickness
                            )
                            *
                            cos(a)
                        ),

                        (float)(
                            cen.y
                            +
                            (
                                std::abs(radius)
                                - thickness
                            )
                            *
                            sin(a)
                        )
                    )
                );
            }

            body.setFillColor(
                sf::Color(70,70,85)
            );

            body.setOutlineThickness(0.08f);

            body.setOutlineColor(
                sf::Color(220,220,255)
            );

            m_window.draw(body);
        }
    }


    void drawGlowLine(
        sf::Vector2f start,
        sf::Vector2f end,
        sf::Color    color
    ) {
        sf::Vector2f d   = end - start;
        float        len = std::sqrt(d.x*d.x + d.y*d.y);

        if (len < 1e-5f)
            return;

        sf::Vector2f perp(-d.y / len, d.x / len);

        struct Layer { float hw; sf::Uint8 a; };

        constexpr Layer layers[] = {
            { 0.50f,  7  },  
            { 0.30f,  18 },
            { 0.16f,  40 },
            { 0.08f,  90 },
            { 0.03f,  210},   
        };

        for (const auto& L : layers) {

            sf::Color gc  = color;
            gc.a          = L.a;

            sf::VertexArray q(sf::Quads, 4);

            q[0] = sf::Vertex(start + perp * L.hw, gc);
            q[1] = sf::Vertex(start - perp * L.hw, gc);
            q[2] = sf::Vertex(end   - perp * L.hw, gc);
            q[3] = sf::Vertex(end   + perp * L.hw, gc);

            m_window.draw(q);
        }
    }

    void drawRays() {

        for (size_t r = 0;
             r < m_rays.size();
             r++)
        {
            if (!m_rays[r].traced)
                continue;

            const auto& segs =
                m_rays[r].path.getSegments();

            float delay =
                r * 5.0f;

            float budget =
                m_animTime - delay;

            if (budget <= 0)
                continue;

            sf::Color color = m_rays[r].color;

            for (size_t i = 0;
                 i < segs.size();
                 i++)
            {
                if (budget <= 0)
                    break;

                float segLen =
                    (float)segs[i].length;

                float drawLen =
                    std::min(segLen, budget);

                budget -= drawLen;

                sf::Vector2f start(
                    (float)segs[i].startPoint.x,
                    (float)segs[i].startPoint.y
                );

                sf::Vector2f end(
                    (float)(
                        segs[i].startPoint.x
                        + segs[i].direction.x * drawLen
                    ),
                    (float)(
                        segs[i].startPoint.y
                        + segs[i].direction.y * drawLen
                    )
                );

                drawGlowLine(start, end, color);

                sf::CircleShape halo(0.50f);
                halo.setOrigin(0.50f, 0.50f);
                halo.setPosition(end);
                sf::Color hc = color;
                hc.a = 100;
                halo.setFillColor(hc);
                m_window.draw(halo);

                sf::CircleShape dot(0.20f);
                dot.setOrigin(0.20f, 0.20f);
                dot.setPosition(end);
                dot.setFillColor(sf::Color::White);
                m_window.draw(dot);
            }
        }
    }
};

#endif