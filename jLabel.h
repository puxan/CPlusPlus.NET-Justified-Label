#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace utils 
{
    /**
     * A user control that works as a multiline label full justified
     */
    public ref class jLabel : public System::Windows::Forms::UserControl
    {
        private:

            String^ text;

        public:

            // Holds the full text to be displayed
            [Localizable (true)]
            property String^ FullText
            {
                virtual String^ get ()
                {
                    return text;
                }

                virtual void set (String ^ value)
                {
                    text = value;
                    splitText ();
                }
            }


            jLabel (void) 
            {
                // Avoid tabulator stop in this control
		TabStop = false;
            }


            /**
             * Force the repaint of all the panel when the size changes.
             *
             * @param e Unused
             */
            virtual void OnSizeChanged (EventArgs^ e) override
            {
                UserControl::OnSizeChanged (e);
                splitText ();
                Invalidate ();
            }

        private:

            /**
             * Split full text into many Labels of one line each, and fill
             * with spaces so that they seem full justified
             */
            void splitText ()
            {
                int numWords = 0;
                int iniLine = 0;
                int lineNum = 0;

                // If there is text
                if (!String::IsNullOrEmpty (text))
                {
                    // Clear previous labels
                    Controls->Clear ();

                    // Split text in an array of words
                    array <String^>^ words = text->Split (gcnew array <Char> { ' ' }, StringSplitOptions::RemoveEmptyEntries);

                    // Until there are words to process
                    while (iniLine < words->Length)
                    {
                        // Determine the number of words that fit in a single line
                        numWords = maxWords (words, iniLine);

                        // Add spaces between words until, with one more space, is longer than the allowed length
                        addLabelWithSpaces (words, iniLine, numWords, lineNum);

                        iniLine += numWords;
                        lineNum ++;
                    }
                }
            }


            /**
             * Get the maximum number of words that fit into the line
             *
             * @param words Array of the words to place into the label
             * @param ini Index of the first word in this line
             * @return Maximum number of words that fit into the line
             */
            int maxWords (array <String^>^ words, int ini)
            {
                int num = 1;
                String^ line = words[ini];
                Drawing::Size lineSize;

                if (ini + num < words->Length)
                {
                    // Until there are no more words or line is too long
                    do
                    {
                        // Add another word to the line
                        line += " " + words[ini + num];
                        lineSize = TextRenderer::MeasureText (line, Font);
                        num ++;
                    }
                    while ((ini + num < words->Length) && (lineSize.Width <= Size.Width));

                    // If we added a word that made line to overflow
                    if (lineSize.Width > Size.Width) num --;
                }
                return num;
            }


            /**
             * Add a label into the object containing a new line.
             * It also add spaces between words to adjust the label width to the object's one.
             *
             * @param words Array of the words to place into the label
             * @param ini Index of the first word in this line
             * @param n Number of words of this line
             * @param lineNum Number of line
             */
            void addLabelWithSpaces (array <String^>^ words, int ini, int n, int lineNum)
            {
                Label^ l = gcnew Label ();
                String^ line = "";
                int spaces = n - 1;
                Drawing::Size lineSize;

                // Build line without extra spaces
                for (int i = ini; i < ini + n; i ++)
                {
                    if (i != ini) line += " ";
                    line += words[i];
                }

                // If is not the last line
                if ((ini + n < words->Length) && (n > 1))
                {
                    // Size of the current line must fit inside the control
                    lineSize = TextRenderer::MeasureText (line, Font);

                    // Add spaces at the front until is bigger than the control size
                    while (lineSize.Width <= Size.Width)
                    {
                        spaces ++;
                        line = " " + line;
                        lineSize = TextRenderer::MeasureText (line, Font);
                    }

                    spaces --;

                    // Distribute spaces
                    int j = 0;
                    array <String^>^ vspaces = gcnew array <String^> (n - 1);
                    for (int i = spaces; i > 0; i --)
                    {
                        vspaces[j] += " ";
                        j = ((j + 1) % (n - 1));
                    }

                    line = "";

                    // Build line without extra spaces
                    for (int i = 0; i < n; i ++)
                    {
                        if (i != 0) line += vspaces[i - 1];
                        line += words[ini + i];
                    }
                }

                // Init new line label
                l->Font = Font;
                l->Text = line;
                l->Size = Drawing::Size (Size.Width, Font->Height);
                l->Location = Point (0, lineNum * Font->Height);

                // Add label to the control
                Controls->Add (l);
            }
    };
}
