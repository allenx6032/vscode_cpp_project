/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
#include <ctype.h>
#include <GLog.h>

#include "GFontFamily.h"

namespace gcanvas
{
    GFontFamilyItem::GFontFamilyItem() {

        font_file_name = nullptr;

        style = GFontStyle::Style::NORMAL;

        variant = GFontStyle::Variant::NORMAL;

        weight = GFontStyle::Weight::NORMAL;

    }


    GFontFamily::GFontFamily(std::list< const char * > &fontFamily) {
        InitFontFamily(fontFamily);
    }


    const char *GFontFamily::MatchFamilyStyle(GFontStyle &fontStyle) {
        GFontStyle::Style style = fontStyle.GetStyle();
        GFontStyle::Weight weight = fontStyle.GetWeight();
        GFontStyle::Variant variant = fontStyle.GetVariant();

        // web属性收敛
        // Android内oblique与italic一样
        if (style == GFontStyle::Style::OBLIQUE) {
            style = GFontStyle::Style::ITALIC;
        }

        if (weight == GFontStyle::Weight::BOLDER || weight == GFontStyle::Weight::EXTRA_BOLD
            || weight == GFontStyle::Weight::SEMI_BOLD) {
            weight = GFontStyle::Weight::BOLD;
        } else if (weight == GFontStyle::Weight::LIGHTER || weight == GFontStyle::Weight::EXTRA_LIGHT) {
            weight = GFontStyle::Weight::LIGHT;
        }

        GFontFamilyItem* closetItem = nullptr;
        for (auto it = fontItems.begin(); it != fontItems.end(); ++it) {
            GFontFamilyItem* item = &(*it);
            // LOG_E("match fontStyle: familyt item=%s, style=%i, weight=%i", item->font_file_name, item->style, item->weight);
            if (item != nullptr) {
                closetItem = item;
                if (item->style == style) {
                    if (closetItem == nullptr) { // 选择首个style匹配的item
                        closetItem = item;
                    }
                    if (item->weight == weight) { // style & weight 都匹配的item
                        closetItem = item;
                        if (item->variant == variant) {
                            break;
                        }
                    }
                }
            }
        }

        if (closetItem != nullptr) {
            // LOG_E("match fontStyle (%s) = file (%s) ", fontStyle.GetFullFontStyle().data(), closetItem->font_file_name);
            return closetItem->font_file_name;
        }
        // LOG_E("match fontStyle (%s) = file empty ", fontStyle.GetFullFontStyle().data());
        return "";
    }


    /**
     * 将xml中的font名称 转化为font family item
     */
    void GFontFamily::InitFontFamily(std::list<const char *> &fontFamily) {
        fontItems.clear();

        // style
        const char *style_italic = "italic";
        // const char *style_oblique = "oblique";

        // variant
        const char *var_smallcaps = "smallcaps";

        // weight（lighter,bolder,extra-light,extra-bold等不支持)
        const char *weight_thin = "thin";
        const char *weight_light = "light";
        const char *weight_regular = "regular";
        const char *weight_medium = "medium";
        const char *weight_bold = "bold";
        const char *weight_black = "black";

        for (auto it = fontFamily.begin(); it != fontFamily.end(); ++it) {
            int length = strlen(*it);
            char *fontFileLowerCase = new char[length + 1];
            strcpy(fontFileLowerCase, *it);

            for (int i = 0; i < length; ++i) {
                fontFileLowerCase[i] = tolower(fontFileLowerCase[i]);
            }

            // new item
            GFontFamilyItem item;

            if (strstr(fontFileLowerCase, style_italic) != nullptr) {
                item.style = GFontStyle::Style::ITALIC;
            }

            if (strstr(fontFileLowerCase, var_smallcaps) != nullptr) {
                item.variant = GFontStyle::Variant::SMALL_CAPS;
            }

            if (strstr(fontFileLowerCase, weight_thin) != nullptr) {
                item.weight = GFontStyle::Weight::THIN;
            } else if (strstr(fontFileLowerCase, weight_light) != nullptr) {
                item.weight = GFontStyle::Weight::LIGHT;
            } else if (strstr(fontFileLowerCase, weight_regular) != nullptr) {
                item.weight = GFontStyle::Weight::NORMAL;
            } else if (strstr(fontFileLowerCase, weight_medium) != nullptr) {
                item.weight = GFontStyle::Weight::MEDIUM;
            } else if (strstr(fontFileLowerCase, weight_bold) != nullptr) {
                item.weight = GFontStyle::Weight::BOLD;
            } else if (strstr(fontFileLowerCase, weight_black) != nullptr) {
                item.weight = GFontStyle::Weight::BLACK;
            }

            item.font_file_name = *it;
            fontItems.push_back(item);

            delete[] fontFileLowerCase;
        }
    }


    char *GFontFamily::GetProperFontFile() {
        GFontFamilyItem* closetItem = nullptr;
        if(fontItems.size() > 0) {
            closetItem = &fontItems[0];
        }
        if (closetItem != nullptr) {
            return (char *)closetItem->font_file_name;
        }
        return "NotoSansCJK-Regular.ttc";
    }

}