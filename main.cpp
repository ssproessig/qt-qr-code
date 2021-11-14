#include "ext/qrcodegen.hpp"

#include <QApplication>
#include <QPainter>
#include <QString>
#include <QSvgWidget>
#include <QTextStream>
#include <QUuid>


namespace
{
QString toSvgString(qrcodegen::QrCode const& qr, int const border)
{
    if (border < 0)
    {
        throw std::domain_error("Border must be non-negative");
    }

    if ((border > (INT_MAX / 2)) || ((border * 2) > (INT_MAX - qr.getSize())))
    {
        throw std::overflow_error("Border too large");
    }

    QString svgString;
    QTextStream str(&svgString);

    auto const bv = qr.getSize() + (border * 2);

    str << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    str << "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\" "
           "\"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">\n";
    str << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" viewBox=\"0 0 ";
    str << bv << " " << bv << "\" stroke=\"none\">\n";
    str << "\t<rect width=\"100%\" height=\"100%\" fill=\"#FFFFFF\"/>\n";
    str << "\t<path d=\"";
    for (int y = 0; y < qr.getSize(); y++)
    {
        for (int x = 0; x < qr.getSize(); x++)
        {
            if (qr.getModule(x, y))
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

} // namespace


int main(int argc, char** argv)
{
    // 1a) prepare the data to encode
    QString const id = "123";
    QString const token = QUuid::createUuid().toString();
    auto const url = QStringLiteral("http://localhost:8080/api/v1/entity/%1/%2").arg(id).arg(token);

    // 1b) use qrcodegen to create SVG
    auto const qr = qrcodegen::QrCode::encodeText(url.toLatin1(), qrcodegen::QrCode::Ecc::MEDIUM);
    auto const svg = toSvgString(qr, 4);


    // 2) use Qt to show the code
    QApplication app(argc, argv); //lint !e1788
    auto sz = 300;
    auto const args = QApplication::arguments();
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
