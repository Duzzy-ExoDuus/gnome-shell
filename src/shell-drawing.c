/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */

#include "shell-drawing.h"
#include <math.h>

/**
 * shell_create_vertical_gradient:
 * @top: the color at the top
 * @bottom: the color at the bottom
 *
 * Creates a vertical gradient actor.
 *
 * Return value: (transfer none): a #ClutterCairoTexture actor with the
 *               gradient. The texture actor is floating, hence (transfer none).
 */
ClutterCairoTexture *
shell_create_vertical_gradient (ClutterColor *top,
                                ClutterColor *bottom)
{
  ClutterCairoTexture *texture;
  cairo_t *cr;
  cairo_pattern_t *pattern;

  /* Draw the gradient on an 8x8 pixel texture. Because the gradient is drawn
   * from the uppermost to the lowermost row, after stretching 1/16 of the
   * texture height has the top color and 1/16 has the bottom color. The 8
   * pixel width is chosen for reasons related to graphics hardware internals.
   */
  texture = CLUTTER_CAIRO_TEXTURE (clutter_cairo_texture_new (8, 8));
  cr = clutter_cairo_texture_create (texture);

  pattern = cairo_pattern_create_linear (0, 0, 0, 8);
  cairo_pattern_add_color_stop_rgba (pattern, 0,
                                     top->red / 255.,
                                     top->green / 255.,
                                     top->blue / 255.,
                                     top->alpha / 255.);
  cairo_pattern_add_color_stop_rgba (pattern, 1,
                                     bottom->red / 255.,
                                     bottom->green / 255.,
                                     bottom->blue / 255.,
                                     bottom->alpha / 255.);

  cairo_set_source (cr, pattern);
  cairo_paint (cr);

  cairo_pattern_destroy (pattern);
  cairo_destroy (cr);

  return texture;
}

/**
 * shell_create_horizontal_gradient:
 * @left: the color on the left
 * @right: the color on the right
 *
 * Creates a horizontal gradient actor.
 *
 * Return value: (transfer none): a #ClutterCairoTexture actor with the
 *               gradient. The texture actor is floating, hence (transfer none).
 */
ClutterCairoTexture *
shell_create_horizontal_gradient (ClutterColor *left,
                                  ClutterColor *right)
{
  ClutterCairoTexture *texture;
  cairo_t *cr;
  cairo_pattern_t *pattern;

  /* Draw the gradient on an 8x1 pixel texture. Because the gradient is drawn
   * from the left to the right column, after stretching 1/16 of the
   * texture width has the left side color and 1/16 has the right side color.
   * There is no reason to use the 8 pixel height that would be similar to the
   * reason we are using the 8 pixel width for the vertical gradient, so we
   * are just using the 1 pixel height instead.
   */
  texture = CLUTTER_CAIRO_TEXTURE (clutter_cairo_texture_new (8, 1));
  cr = clutter_cairo_texture_create (texture);

  pattern = cairo_pattern_create_linear (0, 0, 8, 0);
  cairo_pattern_add_color_stop_rgba (pattern, 0,
                                     left->red / 255.,
                                     left->green / 255.,
                                     left->blue / 255.,
                                     left->alpha / 255.);
  cairo_pattern_add_color_stop_rgba (pattern, 1,
                                     right->red / 255.,
                                     right->green / 255.,
                                     right->blue / 255.,
                                     right->alpha / 255.);

  cairo_set_source (cr, pattern);
  cairo_paint (cr);

  cairo_pattern_destroy (pattern);
  cairo_destroy (cr);

  return texture;
}

void
shell_draw_clock (ClutterCairoTexture *texture,
                  int                  hour,
                  int                  minute)
{
  cairo_t *cr;
  guint width, height;
  double xc, yc, radius, hour_radius, minute_radius;
  double angle;

  clutter_cairo_texture_get_surface_size (texture, &width, &height);
  xc = (double)width / 2;
  yc = (double)height / 2;
  radius = (double)(MIN(width, height)) / 2 - 2;
  minute_radius = radius - 3;
  hour_radius = radius / 2;

  clutter_cairo_texture_clear (texture);
  cr = clutter_cairo_texture_create (texture);
  cairo_set_line_width (cr, 1.0);

  /* Outline */
  cairo_arc (cr, xc, yc, radius, 0.0, 2.0 * M_PI);
  cairo_stroke (cr);

  /* Hour hand. (We add a fraction to @hour for the minutes, then
   * convert to radians, and then subtract pi/2 because cairo's origin
   * is at 3:00, not 12:00.)
   */
  angle = ((hour + minute / 60.0) / 12.0) * 2.0 * M_PI - M_PI / 2.0;
  cairo_move_to (cr, xc, yc);
  cairo_line_to (cr,
                 xc + hour_radius * cos (angle),
                 yc + hour_radius * sin (angle));
  cairo_stroke (cr);

  /* Minute hand */
  angle = (minute / 60.0) * 2.0 * M_PI - M_PI / 2.0;
  cairo_move_to (cr, xc, yc);
  cairo_line_to (cr,
                 xc + minute_radius * cos (angle),
                 yc + minute_radius * sin (angle));
  cairo_stroke (cr);

  cairo_destroy (cr);
}

void
shell_draw_glow (ClutterCairoTexture *texture,
                 double red,
                 double green,
                 double blue,
                 double alpha)
{
  cairo_t *cr;
  guint width, height;
  cairo_pattern_t *gradient;

  clutter_cairo_texture_get_surface_size (texture, &width, &height);

  clutter_cairo_texture_clear (texture);
  cr = clutter_cairo_texture_create (texture);

  cairo_save (cr);
  cairo_translate (cr, width / 2.0, height / 2.0);
  cairo_scale (cr, width / 2.0, height / 2.0);

  gradient = cairo_pattern_create_radial (0.0, 0.0, 0.0, 0.0, 0.0, 1.0);
  cairo_pattern_add_color_stop_rgba (gradient, 0.0, red, green, blue, alpha);
  cairo_pattern_add_color_stop_rgba (gradient, 0.7, red, green, blue, alpha * 0.7);
  cairo_pattern_add_color_stop_rgba (gradient, 1.0, red, green, blue, alpha * 0.3);
  cairo_set_source (cr, gradient);

  cairo_arc (cr, 0.0, 0.0, 1.0, 0.0, 2.0 * M_PI);
  cairo_fill (cr);
  cairo_restore (cr);
  cairo_pattern_destroy (gradient);
  cairo_destroy (cr);
}
