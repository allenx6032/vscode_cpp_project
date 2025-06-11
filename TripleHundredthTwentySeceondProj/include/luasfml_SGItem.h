#ifndef _SG_ITEM_
#define _SG_ITEM_

#include <vector>

#include <SFML_Transformable.hpp>
#include <SFML_RenderTarget.hpp>
#include <SFML_Event.hpp>
#include <SFML_Vector2.hpp>
#include <SFML_Drawable.hpp>
#include <SFML_RectangleShape.hpp>

class SGRessource;

class SGItem :public sf::Drawable, public sf::Transformable
{

public:
	SGItem(SGItem* parent=NULL,sf::Drawable* content=NULL);
	virtual ~SGItem();
	
	float width()const;
	float height()const;
	
	void setWidth(float new_width);
	void setHeight(float new_height);
	
	sf::Drawable* content() const;

	SGItem* parentItem() const;	

	void pick( sf::Vector2f point);
	void release();

	bool containPoint(sf::Vector2f p) const;
	sf::Vector2f mapToItem(const SGItem& finalBasis, sf::Vector2f point) const;
	sf::Vector2f mapFromItem(const SGItem& initialBasis, sf::Vector2f point) const;

	
protected:
	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;

private:
	float m_width;
	float m_height;	
	sf::RectangleShape*        m_debugContent;
	sf::Drawable*              m_content;
	SGItem*                    m_parentItem;
	std::vector<SGItem* >      m_childrensItem;
	mutable sf::Transform      m_globalTransform;
	
	void updateGlobalTransform();

	static SGItem* root;
	static SGItem* pickedItem;

	void recursivePick(std::vector<SGItem*>& items, sf::Vector2f point);


};


#endif // endof _SG_ITEM_
