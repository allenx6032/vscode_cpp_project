//----------------------------------------------------------------------------
// Agg2D - Version 1.0
// Based on Anti-Grain Geometry
// Copyright (C) 2005 Maxim Shemanarev (http://www.antigrain.com)
//
// Permission to copy, use, modify, sell and distribute this software
// is granted provided this copyright notice appears in all copies.
// This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.
//
//----------------------------------------------------------------------------
// Contact: mcseem@antigrain.com
//          mcseemagg@yahoo.com
//          http://www.antigrain.com
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//
// 25 Jan 2007 - Ported to AGG 2.4 Jerry Evans (jerry@novadsp.com)
// 21 June 2023 - Modified to only provide the required API for benchmarks
//
//----------------------------------------------------------------------------

#ifndef AGG2D_INCLUDED
#define AGG2D_INCLUDED

#include "agg_basics.h"
#include "agg_bezier_arc.h"
#include "agg_conv_curve.h"
#include "agg_conv_stroke.h"
#include "agg_conv_transform.h"
#include "agg_font_cache_manager.h"
#include "agg_gamma_functions.h"
#include "agg_image_accessors.h"
#include "agg_path_storage.h"
#include "agg_pixfmt_rgba.h"
#include "agg_rasterizer_scanline_aa.h"
#include "agg_rasterizer_scanline_aa_nogamma.h"
#include "agg_renderer_base.h"
#include "agg_renderer_scanline.h"
#include "agg_rendering_buffer.h"
#include "agg_rounded_rect.h"
#include "agg_scanline_u.h"
#include "agg_span_allocator.h"
#include "agg_span_converter.h"
#include "agg_span_gradient.h"
#include "agg_span_image_filter_rgba.h"
#include "agg_span_interpolator_linear.h"
#include "agg_trans_affine.h"
#include "agg_trans_viewport.h"

class Agg2D
{
    typedef agg::order_bgra ComponentOrder;

    typedef agg::rgba8 ColorType;
    typedef agg::blender_rgba_pre<ColorType, ComponentOrder> BlenderPre;
    typedef agg::comp_op_adaptor_rgba_pre<ColorType, ComponentOrder> BlenderCompPre;

    typedef agg::pixfmt_bgra32_pre PixFormatPre;
    typedef agg::pixfmt_custom_blend_rgba<BlenderCompPre,agg::rendering_buffer> PixFormatCompPre;

    typedef agg::renderer_base<PixFormatPre> RendererBasePre;
    typedef agg::renderer_base<PixFormatCompPre> RendererBaseCompPre;

    typedef agg::renderer_scanline_aa_solid<RendererBasePre> RendererSolid;
    typedef agg::renderer_scanline_aa_solid<RendererBaseCompPre> RendererSolidComp;

    typedef agg::span_allocator<ColorType> SpanAllocator;
    typedef agg::pod_auto_array<ColorType, 256> GradientArray;

    typedef agg::span_gradient<ColorType, agg::span_interpolator_linear<>, agg::gradient_x,      GradientArray> LinearGradientSpan;
    typedef agg::span_gradient<ColorType, agg::span_interpolator_linear<>, agg::gradient_circle, GradientArray> RadialGradientSpan;
    typedef agg::span_gradient<ColorType, agg::span_interpolator_linear<>, agg::gradient_conic , GradientArray> ConicGradientSpan;

    typedef agg::conv_curve<agg::path_storage> ConvCurve;
    typedef agg::conv_stroke<ConvCurve> ConvStroke;
    typedef agg::conv_transform<ConvCurve> PathTransform;
    typedef agg::conv_transform<ConvStroke> StrokeTransform;

    enum StyleFlag
    {
        None,
        Solid,
        Linear,
        Radial
    };

public:
    friend class Agg2DRenderer;

    typedef ColorType         Color;
    typedef agg::rect_i       Rect;
    typedef agg::rect_d       RectD;
    typedef agg::trans_affine Affine;

    enum StyleSlot
    {
        FillSlot,
        StrokeSlot
    };

    enum LineJoin
    {
        JoinMiter = agg::miter_join,
        JoinRound = agg::round_join,
        JoinBevel = agg::bevel_join
    };

    enum LineCap
    {
        CapButt   = agg::butt_cap,
        CapSquare = agg::square_cap,
        CapRound  = agg::round_cap
    };

    enum DrawPathFlag
    {
        FillOnly,
        StrokeOnly,
        FillAndStroke,
        FillWithLineColor
    };

    struct Transformations
    {
        double affineMatrix[6];
    };

    struct Image
    {
        agg::rendering_buffer renBuf;

        Image() {}
        Image(unsigned char* buf, unsigned width, unsigned height, int stride) :
            renBuf(buf, width, height, stride) {}
        void attach(unsigned char* buf, unsigned width, unsigned height, int stride)
        {
            renBuf.attach(buf, width, height, stride);
        }
        int width()  const { return renBuf.width(); }
        int height() const { return renBuf.height(); }
    };

    enum ImageFilter
    {
        NoFilter,
        Bilinear
    };

    enum ImageResample
    {
        NoResample,
        ResampleAlways,
        ResampleOnZoomOut
    };

    enum BlendMode
    {
        BlendAlpha      = agg::end_of_comp_op_e,
        BlendClear      = agg::comp_op_clear,
        BlendSrc        = agg::comp_op_src,
        BlendDst        = agg::comp_op_dst,
        BlendSrcOver    = agg::comp_op_src_over,
        BlendDstOver    = agg::comp_op_dst_over,
        BlendSrcIn      = agg::comp_op_src_in,
        BlendDstIn      = agg::comp_op_dst_in,
        BlendSrcOut     = agg::comp_op_src_out,
        BlendDstOut     = agg::comp_op_dst_out,
        BlendSrcAtop    = agg::comp_op_src_atop,
        BlendDstAtop    = agg::comp_op_dst_atop,
        BlendXor        = agg::comp_op_xor,
        BlendAdd        = agg::comp_op_plus,
        BlendMultiply   = agg::comp_op_multiply,
        BlendScreen     = agg::comp_op_screen,
        BlendOverlay    = agg::comp_op_overlay,
        BlendDarken     = agg::comp_op_darken,
        BlendLighten    = agg::comp_op_lighten,
        BlendColorDodge = agg::comp_op_color_dodge,
        BlendColorBurn  = agg::comp_op_color_burn,
        BlendHardLight  = agg::comp_op_hard_light,
        BlendSoftLight  = agg::comp_op_soft_light,
        BlendDifference = agg::comp_op_difference,
        BlendExclusion  = agg::comp_op_exclusion
    };

    enum Direction
    {
        CW, CCW
    };

    ~Agg2D();
    Agg2D();

    // Setup
    // -----

    void  attach(unsigned char* buf, unsigned width, unsigned height, int stride);
    void  attach(Image& img);

    void  clipBox(double x1, double y1, double x2, double y2);
    RectD clipBox() const;

    void  clearAll(Color c);
    void  clearAll(unsigned r, unsigned g, unsigned b, unsigned a = 255);

    // Conversions
    // -----------

    void   worldToScreen(double& x, double& y) const;
    void   screenToWorld(double& x, double& y) const;
    double worldToScreen(double scalar) const;
    double screenToWorld(double scalar) const;
    void   alignPoint(double& x, double& y) const;
    bool   inBox(double worldX, double worldY) const;

    // General Attributes
    // ------------------

    void blendMode(BlendMode m);
    BlendMode blendMode() const;

    void imageBlendMode(BlendMode m);
    BlendMode imageBlendMode() const;

    void imageBlendColor(Color c);
    void imageBlendColor(unsigned r, unsigned g, unsigned b, unsigned a = 255);
    Color imageBlendColor() const;

    void masterAlpha(double a);
    double masterAlpha() const;

    void fillColor(Color c);
    void fillColor(unsigned r, unsigned g, unsigned b, unsigned a = 255);
    void noFill();

    void lineColor(Color c);
    void lineColor(unsigned r, unsigned g, unsigned b, unsigned a = 255);
    void noLine();

    void setLinearGradient(StyleSlot slot, double x1, double y1, double x2, double y2, Color c1, Color c2, Color c3);
    void fillLinearGradient(double x1, double y1, double x2, double y2, Color c1, Color c2, Color c3);
    void lineLinearGradient(double x1, double y1, double x2, double y2, Color c1, Color c2, Color c3);

    void setRadialGradient(StyleSlot slot, double x, double y, double r, Color c1, Color c2, Color c3);
    void fillRadialGradient(double x, double y, double r, Color c1, Color c2, Color c3);
    void lineRadialGradient(double x, double y, double r, Color c1, Color c2, Color c3);

    void lineWidth(double w);
    double lineWidth(double w) const;

    void lineCap(LineCap cap);
    LineCap lineCap() const;

    void lineJoin(LineJoin join);
    LineJoin lineJoin() const;

    void fillEvenOdd(bool evenOddFlag);
    bool fillEvenOdd() const;

    // Transformations
    // ---------------

    Transformations transformations() const;
    void transformations(const Transformations& tr);
    void resetTransformations();
    void affine(const Affine& tr);
    void affine(const Transformations& tr);
    void rotate(double angle);
    void scale(double sx, double sy);
    void skew(double sx, double sy);
    void translate(double x, double y);

    // Basic Shapes
    // ------------

    void fillRectangleI(int x1, int y1, int x2, int y2, Color color);

    void line(double x1, double y1, double x2, double y2);
    void triangle(double x1, double y1, double x2, double y2, double x3, double y3);
    void rectangle(double x1, double y1, double x2, double y2);
    void roundedRect(double x1, double y1, double x2, double y2, double r);
    void roundedRect(double x1, double y1, double x2, double y2, double rx, double ry);
    void roundedRect(double x1, double y1, double x2, double y2,
                     double rxBottom, double ryBottom,
                     double rxTop,    double ryTop);
    void ellipse(double cx, double cy, double rx, double ry);
    void arc(double cx, double cy, double rx, double ry, double start, double sweep);
    void star(double cx, double cy, double r1, double r2, double startAngle, int numRays);
    void curve(double x1, double y1, double x2, double y2, double x3, double y3);
    void curve(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4);
    void polygon(double* xy, int numPoints);
    void polyline(double* xy, int numPoints);


    // Path commands
    // -------------

    void resetPath();

    void moveTo(double x, double y);
    void moveRel(double dx, double dy);

    void lineTo(double x, double y);
    void lineRel(double dx, double dy);

    void horLineTo(double x);
    void horLineRel(double dx);

    void verLineTo(double y);
    void verLineRel(double dy);

    void arcTo(double rx, double ry,
               double angle,
               bool largeArcFlag,
               bool sweepFlag,
               double x, double y);

    void arcRel(double rx, double ry,
                double angle,
                bool largeArcFlag,
                bool sweepFlag,
                double dx, double dy);

    void quadricCurveTo(double xCtrl, double yCtrl,
                         double xTo,   double yTo);
    void quadricCurveRel(double dxCtrl, double dyCtrl,
                         double dxTo,   double dyTo);
    void quadricCurveTo(double xTo, double yTo);
    void quadricCurveRel(double dxTo, double dyTo);

    void cubicCurveTo(double xCtrl1, double yCtrl1,
                      double xCtrl2, double yCtrl2,
                      double xTo,    double yTo);

    void cubicCurveRel(double dxCtrl1, double dyCtrl1,
                       double dxCtrl2, double dyCtrl2,
                       double dxTo,    double dyTo);

    void cubicCurveTo(double xCtrl2, double yCtrl2,
                      double xTo,    double yTo);

    void cubicCurveRel(double xCtrl2, double yCtrl2,
                       double xTo,    double yTo);

    void addEllipse(double cx, double cy, double rx, double ry, Direction dir);
    void closePolygon();

    void drawPath(DrawPathFlag flag = FillAndStroke);
    void drawPathNoTransform(DrawPathFlag flag = FillAndStroke);


    // Image Transformations
    // ---------------------

    void imageFilter(ImageFilter f);
    ImageFilter imageFilter() const;

    void imageResample(ImageResample f);
    ImageResample imageResample() const;

    void transformImage(const Image& img,
                           int imgX1,    int imgY1,    int imgX2,    int imgY2,
                        double dstX1, double dstY1, double dstX2, double dstY2);

    void transformImage(const Image& img,
                        double dstX1, double dstY1, double dstX2, double dstY2);

    void transformImage(const Image& img,
                        int imgX1, int imgY1, int imgX2, int imgY2,
                        const double* parallelogram);

    void transformImage(const Image& img, const double* parallelogram);


    void transformImagePath(const Image& img,
                               int imgX1,    int imgY1,    int imgX2,    int imgY2,
                            double dstX1, double dstY1, double dstX2, double dstY2);

    void transformImagePath(const Image& img,
                            double dstX1, double dstY1, double dstX2, double dstY2);

    void transformImagePath(const Image& img,
                            int imgX1, int imgY1, int imgX2, int imgY2,
                            const double* parallelogram);

    void transformImagePath(const Image& img, const double* parallelogram);


    // Image Blending (no transformations available)
    void blendImage(Image& img,
                    int imgX1, int imgY1, int imgX2, int imgY2,
                    double dstX, double dstY, unsigned alpha=255);
    void blendImage(Image& img, double dstX, double dstY, unsigned alpha=255);


    // Copy image directly, together with alpha-channel
    void copyImage(Image& img,
                   int imgX1, int imgY1, int imgX2, int imgY2,
                   double dstX, double dstY);
    void copyImage(Image& img, double dstX, double dstY);


    // Auxiliary
    // ---------

    static double pi() { return agg::pi; }
    static double deg2Rad(double v) { return v * agg::pi / 180.0; }
    static double rad2Deg(double v) { return v * 180.0 / agg::pi; }

private:
    void render(StyleSlot slot);
    void addLine(double x1, double y1, double x2, double y2);
    void renderImage(const Image& img, int x1, int y1, int x2, int y2, const double* parl);

    agg::rendering_buffer           m_rbuf;
    PixFormatPre                    m_pixFormatPre;
    PixFormatCompPre                m_pixFormatCompPre;
    RendererBasePre                 m_renBasePre;
    RendererBaseCompPre             m_renBaseCompPre;
    RendererSolid                   m_renSolid;
    RendererSolidComp               m_renSolidComp;

    SpanAllocator                   m_allocator;
    RectD                           m_clipBox;

    BlendMode                       m_blendMode;
    BlendMode                       m_imageBlendMode;
    Color                           m_imageBlendColor;

    agg::scanline_u8                m_scanline;
    agg::rasterizer_scanline_aa_nogamma<> m_rasterizer;
    double                          m_masterAlpha;

    Color                           m_color[2];
    GradientArray                   m_gradient[2];

    LineCap                         m_lineCap;
    LineJoin                        m_lineJoin;

    StyleFlag                       m_styleFlag[2];
    agg::trans_affine               m_gradientMatrix[2];
    double                          m_gradientD1[2];
    double                          m_gradientD2[2];

    ImageFilter                     m_imageFilter;
    ImageResample                   m_imageResample;
    agg::image_filter_lut           m_imageFilterLut;

    agg::span_interpolator_linear<> m_gradientInterpolator[2];

    agg::gradient_x                 m_linearGradientFunction;
    agg::gradient_circle            m_radialGradientFunction;

    double                          m_lineWidth;
    bool                            m_evenOddFlag;

    agg::path_storage               m_path;
    agg::trans_affine               m_transform;

    ConvCurve                       m_convCurve;
    ConvStroke                      m_convStroke;

    PathTransform                   m_pathTransform;
    StrokeTransform                 m_strokeTransform;
};


inline bool operator==(const Agg2D::Color& c1, const Agg2D::Color& c2)
{
   return c1.r == c2.r && c1.g == c2.g && c1.b == c2.b && c1.a == c2.a;
}

inline bool operator!=(const Agg2D::Color& c1, const Agg2D::Color& c2)
{
   return !(c1 == c2);
}

#endif
