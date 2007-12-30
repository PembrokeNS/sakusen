#include "pngimage.h"

#include "fileioexn.h"

#include <png.h>

namespace sakusen {
namespace resources {

/** \internal \brief Private implementation class to help with PNG loading.
 *
 * PNG loading is fairly long and complex, and this class helps make it RAII.
 * It also makes it unnecessary to #include <png.h> in pngimage.h. */
class PngImageImpl {
  public:
    PngImageImpl(const boost::filesystem::path& fileName);
    ~PngImageImpl();
  private:
    FILE* fp;
    png_structp pngStruct;
    png_infop info;
    png_infop endInfo;
    uint32 width;
    uint32 height;
    int bitDepth;
    uint8** rowPointers;
  public:
    void load();
    void read(boost::multi_array<uint16, 2>& out);
};

PngImageImpl::PngImageImpl(const boost::filesystem::path& path) :
  fp(NULL),
  pngStruct(NULL),
  info(NULL),
  endInfo(NULL),
  width(0),
  height(0),
  bitDepth(0),
  rowPointers(NULL)
{
#ifndef _MSC_VER
  fp = fopen(path.native_file_string().c_str(), "rb");
  if (!fp) {
    throw FileIOExn("fopen");
  }
#else
  if (fopen_s(&fp, path.native_file_string().c_str(), "rb"))
    throw FileIOExn("fopen");
#endif

#define SIG_BYTES 8
  /* Read in a bit of file and check it matches the PNG header */
  uint8 header[SIG_BYTES];
  fread(header, 1, SIG_BYTES, fp);
  if (png_sig_cmp(header, 0, SIG_BYTES)) {
    throw FileIOExn::customMessage("file not a PNG");
  }

  pngStruct = png_create_read_struct(
      PNG_LIBPNG_VER_STRING, NULL, NULL, NULL
    );
  if (!pngStruct) {
    throw FileIOExn::customMessage("png_create_read_struct failed");
  }

  info = png_create_info_struct(pngStruct);
  if (!info) {
    png_destroy_read_struct(&pngStruct, NULL, NULL);
    throw FileIOExn::customMessage("png_create_info_struct failed");
  }

  endInfo = png_create_info_struct(pngStruct);
  if (!endInfo) {
    png_destroy_read_struct(&pngStruct, &info, NULL);
    throw FileIOExn::customMessage("png_create_info_struct failed");
  }
}

PngImageImpl::~PngImageImpl()
{
  png_destroy_read_struct(&pngStruct, &info, &endInfo);
  if (fp != NULL) {
    fclose(fp);
  }
}

void PngImageImpl::load()
{
  /* libpng error handling uses setjmp (No, really!!) */
  if (setjmp(png_jmpbuf(pngStruct))) {
    throw FileIOExn::customMessage("PNG read failed");
  }

  /* For simplicity we use a FILE*; more complex methods could allow input in
   * other formats (e.g. straight out of memory) */
  png_init_io(pngStruct, fp);

  png_set_sig_bytes(pngStruct, SIG_BYTES);

  /* We could set up callbacks for:
   * - Handling unknown chunks
   * - Indicating progress
   * And we can do other things related to handling of unknown chunks, but it's
   * probably not needed.
   */

  /* Tell libpng that we really want to allow images with sizes greater than
   * its arbitrary 1-million pixel limit (2^31-1 is the largest allowed by the
   * standard) */
  png_set_user_limits(pngStruct, 0x7fffffffL, 0x7fffffffL);

  /* We use the "high-level" interface because we want to read the entire image
   * into memory with no notable transforms.
   * PNG_TRANSFORM_PACKING will promote sub-byte pixels to full bytes
   */
  png_read_png(
      pngStruct, info, PNG_TRANSFORM_PACKING, NULL
    );

  /* And then we fetch out the loaded data */
  rowPointers = png_get_rows(pngStruct, info);

  /* And we also get the metadata */
  png_uint_32 localWidth;
  png_uint_32 localHeight;
  int colourType;

  png_get_IHDR(
      pngStruct, info, &localWidth, &localHeight, &bitDepth, &colourType,
      NULL, NULL, NULL
    );

  width = localWidth;
  height = localHeight;

  /* For now we handle only greyscale images */
  if (colourType != PNG_COLOR_TYPE_GRAY) {
    throw FileIOExn::customMessage("PNG wasn't greyscale");
  }
}

void PngImageImpl::read(boost::multi_array<uint16, 2>& out)
{
  out.resize(boost::extents[width][height]);
  if (bitDepth == 16) {
    /* pixels come in pairs of bytes, in *big-endian* order */
    for (size_t i=0; i<height; ++i) {
      for (size_t j=0; j<width; ++j) {
        out[j][i] = (rowPointers[i][2*j] << 8) + rowPointers[i][2*j+1];
      }
    }
  } else {
    for (size_t i=0; i<height; ++i) {
      for (size_t j=0; j<width; ++j) {
        /* We scale up 8- (and 4- and 2-) bit values to be on the same scale
         * as the 16-bit ones */
        out[j][i] = rowPointers[i][j]<<8;
      }
    }
  }
}

/** \brief Opens a PNG image and loads it into memory.
 *
 * \param path Location of PNG image on disk
 *
 * Throws a FileIOExn if anything goes wrong (such as the PNG being corrupted).
 */
PngImage::PngImage(const boost::filesystem::path& path) :
  impl(new PngImageImpl(path))
{
  impl->load();
}

/** \brief Destructor.
 *
 * \note The default constructor won't do because PngImageImpl needs to be a
 * complete type at destruction time.
 */
PngImage::~PngImage() {}

void PngImage::read(boost::multi_array<uint16, 2>& out) const
{
  impl->read(out);
}

}}
