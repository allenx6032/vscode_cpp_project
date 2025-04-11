#include "litehtml_html.h"
#include "litehtml_el_title.h"
#include "litehtml_document.h"

litehtml::el_title::el_title(const std::shared_ptr<document>& doc) : html_tag(doc)
{

}

void litehtml::el_title::parse_attributes()
{
	string text;
	get_text(text);
	get_document()->container()->set_caption(text.c_str());
}

