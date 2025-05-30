#ifndef __TEXTBOX_H__
#define	__TEXTBOX_H__

#include <darnit_api.h>

#define	DEFAULT_TEXT_SPEED	30


#ifdef _EMIT_PALETTE
static unsigned char textbox_color_palette[1024] = {
	/* R, G, B, A */
	0x00, 0x00, 0x00, 0xFF,
	0x00, 0x00, 0xAA, 0xFF,
	0x00, 0xAA, 0x00, 0xFF,
	0x00, 0xAA, 0xAA, 0xFF,
	0xAA, 0x00, 0x00, 0xFF,
	0xAA, 0x00, 0xAA, 0xFF,
	0xAA, 0x55, 0x00, 0xFF,
	0xAA, 0xAA, 0xAA, 0xFF,
	0x55, 0x55, 0x55, 0xFF,
	0x55, 0x55, 0xAA, 0xFF,
	0xAA, 0x55, 0xAA, 0xFF,
	0x55, 0xFF, 0xFF, 0xFF,
	0xFF, 0x55, 0x55, 0xFF,
	0xFF, 0x55, 0xFF, 0xFF,
	0xFF, 0xFF, 0x55, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF,
};
#endif


struct textbox {
	char			*message;
	unsigned int		char_pos;
	unsigned int		row;
	unsigned int		rows;
	char			*option;
	unsigned int		options;
	DARNIT_TILESHEET	*face_ts;
	DARNIT_TILE		*face;
	int			face_id;
	/* Textbox */
	DARNIT_TILEMAP		*tc;
	DARNIT_TEXT_SURFACE	*text;

	/* Question box */
	DARNIT_TILEMAP		*qt;
	DARNIT_TEXT_SURFACE	*qts;
	DARNIT_TILE		*pointer;

	int			selection;
	int			y_selection;

	unsigned int		surface_w;
	unsigned int		current_surface_w;
	unsigned int		pad_start;

	int			x;
	int			y;
	int			w;
	int			h;

	int			ms_per_char;
	int			dt;
	int			char_pingback;
};

void textbox_init(int w, int h, int x, int y, int pad_x, int pad_y, int pad_x2, int pad_y2);
void textbox_loop();
void textbox_add_message(const char *message, const char *question, int face, int pingback);
void textbox_draw();
void textbox_destroy();


#endif
