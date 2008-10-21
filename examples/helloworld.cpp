#include "SkBitmap.h"
#include "SkDevice.h"
#include "SkPaint.h"
#include "SkRect.h"
#include <cairo/cairo.h>

int main()
{
    SkBitmap bitmap;
    bitmap.setConfig(SkBitmap::kARGB_8888_Config, 100, 100);
    bitmap.allocPixels();
    SkDevice device(bitmap);
    SkCanvas canvas(&device);
    SkPaint paint;
    SkRect r;

    paint.setARGB(255, 255, 255, 255);
    r.set(10, 10, 20, 20);
    canvas.drawRect(r, paint);

    paint.setARGB(255, 255, 0, 0);
    r.offset(5, 5);
    canvas.drawRect(r, paint);

    paint.setARGB(255, 0, 0, 255);
    r.offset(5, 5);
    canvas.drawRect(r, paint);

    {
	SkAutoLockPixels image_lock(bitmap);
	cairo_surface_t *surface = 
		cairo_image_surface_create_for_data(
			(unsigned char *) bitmap.getPixels(), 
			CAIRO_FORMAT_ARGB32,
			bitmap.width(), 
			bitmap.height(), 
			bitmap.rowBytes());


	cairo_surface_write_to_png(surface, "snapshot.png");
	cairo_surface_destroy(surface);
    }

    return 0;
}
