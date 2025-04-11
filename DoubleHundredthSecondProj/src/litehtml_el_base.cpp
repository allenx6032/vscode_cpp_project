#include "litehtml_html.h"
#include "litehtml_el_base.h"
#include "litehtml_document.h"

litehtml::el_base::el_base(const std::shared_ptr<document>& doc) : html_tag(doc)
{
	
}

void litehtml::el_base::parse_attributes()
{
	get_document()->container()->set_base_url(get_attr("href"));
}
