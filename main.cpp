#ifdef USE_C_API
#include "ext/qrcodegen.h"
#else
#include "ext/qrcodegen.hpp"
#endif

#include <QApplication>
#include <QPainter>
#include <QString>
#include <QSvgWidget>
#include <QTextStream>
#include <QUuid>

#include <stdexcept>

#ifndef QStringLiteral
#define QStringLiteral QString
#endif


namespace
{

struct AbstractQrCode
{
    virtual ~AbstractQrCode() {}
    virtual QString getSvgFor(QString const& anUrl) = 0;
    virtual bool getModule(int x, int y) const = 0;

    QString toSvgString(int const border, int const size)
    {
        if (border < 0)
        {
            throw std::domain_error("Border must be non-negative");
        }

        if ((border > (INT_MAX / 2)) || ((border * 2) > (INT_MAX - size)))
        {
            throw std::overflow_error("Border too large");
        }

        QString svgString;
        QTextStream str(&svgString);

        int const bv = size + (border * 2);

        str << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
        str << "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\" "
               "\"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">\n";
        str << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" viewBox=\"0 0 ";
        str << bv << " " << bv << "\" stroke=\"none\">\n";
        str << "\t<rect width=\"100%\" height=\"100%\" fill=\"#FFFFFF\"/>\n";
        str << "\t<path d=\"";

        for (int y = 0; y < size; y++)
        {
            for (int x = 0; x < size; x++)
            {
                if (getModule(x, y))
                {
                    if ((x != 0) || (y != 0))
                    {
                        str << " ";
                    }

                    str << "M" << (x + border) << "," << (y + border) << "h1v1h-1z";
                }
            }
        }

        str << "\" fill=\"#000000\"/>\n";
        str << "</svg>\n";

        return svgString;
    }
};

#ifdef USE_C_API
struct QrCApi : AbstractQrCode
{
    uint8_t qrcode[qrcodegen_BUFFER_LEN_MAX];

    QString getSvgFor(QString const& anUrl)
    {
        memset(&qrcode, 0, qrcodegen_BUFFER_LEN_MAX);
        uint8_t tempBuffer[qrcodegen_BUFFER_LEN_MAX];

        bool ok = qrcodegen_encodeText(anUrl.toLatin1(),
                                       tempBuffer,
                                       qrcode,
                                       qrcodegen_Ecc_LOW,
                                       qrcodegen_VERSION_MIN,
                                       qrcodegen_VERSION_MAX,
                                       qrcodegen_Mask_AUTO,
                                       true);

        return ok ? toSvgString(4, qrcodegen_getSize(qrcode)) : "";
    }

    bool getModule(int x, int y) const
    {
        return qrcodegen_getModule(qrcode, x, y);
    }
};
#else
struct QrCppApi : AbstractQrCode
{
    qrcodegen::QrCode qr;

    QrCppApi() : qr(qrcodegen::QrCode::encodeText("", qrcodegen::QrCode::Ecc::LOW)) {}

    QString getSvgFor(QString const& anUrl)
    {
        qr = qrcodegen::QrCode::encodeText(anUrl.toLatin1(), qrcodegen::QrCode::Ecc::MEDIUM);
        return toSvgString(4, qr.getSize());
    }

    bool getModule(int x, int y) const
    {
        return qr.getModule(x, y);
    }
};
#endif

} // namespace


int main(int argc, char** argv)
{
    // 1a) prepare the data to encode
    QString const id = "123";
    QString const token = QUuid::createUuid().toString();
    QString const url =
            QStringLiteral("http://localhost:8080/api/v1/entity/%1/%2").arg(id).arg(token);

    // 1b) use qrcodegen to create SVG
#ifdef USE_C_API
    QrCApi api;
#else
    QrCppApi api;
#endif

    QString const svg = api.getSvgFor(url);

    // 2) use Qt to show the code
    QApplication app(argc, argv); //lint !e1788
    int sz = 300;
    QStringList const args = QApplication::arguments();
    if (args.count() > 1)
    {
        sz = args.at(1).toInt();
    }

    QSvgWidget w;
    w.load(svg.toUtf8());
    w.resize(sz, sz);
    w.show();

    return QApplication::exec();
}
