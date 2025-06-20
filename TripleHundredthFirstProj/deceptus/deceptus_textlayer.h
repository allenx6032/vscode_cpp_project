#ifndef TEXTLAYER_H
#define TEXTLAYER_H

#include "deceptus_gamedeserializedata.h"
#include "deceptus_bitmapfont.h"
#include "deceptus_gamenode.h"
#include "deceptus_gamemechanism.h"

#include <SFML_Graphics.hpp>

struct TmxObject;

struct TextLayer : public GameMechanism, public GameNode
{
public:
   TextLayer(GameNode* parent = nullptr);
   void draw(sf::RenderTarget& target, sf::RenderTarget& normal) override;
   void update(const sf::Time& dt) override;
   std::optional<sf::FloatRect> getBoundingBoxPx() override;
   static std::shared_ptr<TextLayer> deserialize(GameNode* parent, const GameDeserializeData& data);

private:
   enum class Mode
   {
      Invalid,
      Bitmap,
      TrueType
   };

   Mode _mode = Mode::Invalid;

   sf::FloatRect _rect;
   std::shared_ptr<sf::Texture> _texture;

   BitmapFont _bitmap_font;
   std::vector<std::shared_ptr<sf::IntRect>> _bitmap_coords;

   std::string _text;
   sf::Font _truetype_font;
   std::unique_ptr<sf::Text> _truetype_text;
};

#endif  // TEXTLAYER_H
