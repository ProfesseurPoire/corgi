#pragma once

namespace corgi
{
    class Shader
    {
    public:

        enum class Stage
        {
            Vertex,
            Fragment
        };

        Shader()=default;
        Shader(Stage stage)
            : stage_(stage)
        {
        }

        ~Shader()=default;

        [[nodiscard]] Stage stage()const noexcept;

    private:

        Stage stage_;

    };
}