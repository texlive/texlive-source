/*
 * Copyright (C) 2008, Pino Toscano <pino@kde.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef DOCUMENTOBSERVER_H
#define DOCUMENTOBSERVER_H

class PdfViewer;
namespace Poppler {
class Document;
}

class DocumentObserver
{
friend class PdfViewer;

public:
    virtual ~DocumentObserver();

    virtual void documentLoaded() = 0;
    virtual void documentClosed() = 0;
    virtual void pageChanged(int page) = 0;

protected:
    DocumentObserver();

    Poppler::Document* document() const;
    void setPage(int page);
    int page() const;
    void reloadPage();

private:
    PdfViewer *m_viewer;
};

#endif
