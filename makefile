# Hauptdokument
MAINFILE = main

# Welche Eigenschaften hat das Dokument
# index = Index/Stichwortverzeichnis
# abbr = Abkuerzungsverzeichnis
# bib = Literaturverzeichnis
FEATURES = index abbr bib

# Compiler fuer das LaTeX-Dokument
LATEX = pdflatex
LATEXFLAGS = -interaction=nonstopmode

# Programm zum Erstellen des Index/Abkuerzungsverzeichnis
MAKEINDEX = makeindex

# Programm zum Erstellen des Literaturverzeichnis
BIBTEX = bibtex

# Programm zum loeschen der Dateien
# DEL = del
DEL = rm -f

# Programm um Meldungen auf der Konsole ausgeben zu koennen
ECHO = echo

# Programm zum oeffnen des PDF-Dokuments
# VIEWPDF = cmd /c start
VIEWPDF = evince

# Sicherheitskopien
BACKUPSUFFIX = ~
BACKUPFILES = $(patsubst %.tex,%.tex$(BACKUPSUFFIX),$(wildcard *.tex)) $(patsubst %.bib,%.bib$(BACKUPSUFFIX),$(wildcard *.bib))

.PHONY: help clean cleanaux cleanbackup cleanall cleanpdf showpdf

# Regeln
all: $(MAINFILE).pdf
index: $(MAINFILE).ind $(MAINFILE).ilg
abbr: $(MAINFILE).nls
bib: $(MAINFILE).blg $(MAINFILE).bbl

# Hilfe ausgeben
help:
	@$(ECHO) Regeln
	@$(ECHO) ------
	@$(ECHO) all        : Komplettes PDF-Dokument mit allen Features erstellen
	@$(ECHO) pdf        : PDF-Dokument ohne Features erstellen
	@$(ECHO) index      : Index/Stichwortverzeichnis erstellen
	@$(ECHO) abbr       : Abkuerzungsverzeichnis erstellen
	@$(ECHO) bib        : Literaturverzeichnis erstellen
	@$(ECHO) clean      : temporaere Dateien loeschen
	@$(ECHO) cleanbackup: Sicherheitskopien loeschen
	@$(ECHO) cleanpdf   : PDF-Dokument loeschen
	@$(ECHO) cleanall   : Fuehre clean, cleanbackup und cleanpdf aus
	@$(ECHO) showpdf    : PDF-Dokument anzeigen
	@$(ECHO)
	@$(ECHO) Infos
	@$(ECHO) -----
	@$(ECHO) Hauptdokument: $(MAINFILE).tex
	@$(ECHO) Features     : $(FEATURES)
	@$(ECHO)
 
# PDF-Dokument
pdf: $(MAINFILE).tex
	@$(ECHO) ======================================================================
	@$(ECHO) PDF-Dokument
	@$(ECHO) ======================================================================
	$(LATEX) $(LATEXFLAGS) $(MAINFILE).tex

# Komplettes PDF-Dokument (Regel all)
$(MAINFILE).pdf: $(MAINFILE).tex $(FEATURES)
	@$(ECHO) ======================================================================
	@$(ECHO) Komplettes PDF-Dokument: 1. Durchgang
	@$(ECHO) ======================================================================
	$(LATEX) $(LATEXFLAGS) $(MAINFILE).tex
	
	@$(ECHO) ======================================================================
	@$(ECHO) Komplettes PDF-Dokument: 2. Durchgang
	
	@$(ECHO) ======================================================================

	$(LATEX) $(LATEXFLAGS) $(MAINFILE).tex

# Vorbereitung
$(MAINFILE).idx $(MAINFILE).nlo $(MAINFILE).aux: $(MAINFILE).tex
	@$(ECHO) ======================================================================
	@$(ECHO) Vorbereitung
	@$(ECHO) ======================================================================
	$(LATEX) $(LATEXFLAGS) $(MAINFILE).tex

# Index/Stichwortverzeichnis (Regel index)
$(MAINFILE).ind $(MAINFILE).ilg: $(MAINFILE).tex $(MAINFILE).idx
	@$(ECHO) ======================================================================
	@$(ECHO) Index
	@$(ECHO) ======================================================================
	$(MAKEINDEX) $(MAINFILE).idx

# Abkuerzungsverzeichnis (Regel abbr)
$(MAINFILE).nls: $(MAINFILE).tex $(MAINFILE).nlo
	@$(ECHO) ======================================================================
	@$(ECHO) Abkuerzungsverzeichnis
	@$(ECHO) ======================================================================
	$(MAKEINDEX) $(MAINFILE).nlo -s nomencl.ist -o $(MAINFILE).nls

# Literaturverzeichnis (Regel bib)
$(MAINFILE).blg $(MAINFILE).bbl: $(MAINFILE).tex $(MAINFILE).bib $(MAINFILE).aux
	@$(ECHO) ======================================================================
	@$(ECHO) Literaturverzeichnis
	@$(ECHO) ======================================================================
	$(BIBTEX) $(MAINFILE)

# Temporaere Dateien loeschen
clean:
	$(DEL) $(MAINFILE).aux
	$(DEL) $(MAINFILE).bbl
	$(DEL) $(MAINFILE).bit
	$(DEL) $(MAINFILE).blg
	$(DEL) $(MAINFILE).dvi
	$(DEL) $(MAINFILE).glo
	$(DEL) $(MAINFILE).idx
	$(DEL) $(MAINFILE).ilg
	$(DEL) $(MAINFILE).ilg
	$(DEL) $(MAINFILE).ind
	$(DEL) $(MAINFILE).lof
	$(DEL) $(MAINFILE).log
	$(DEL) $(MAINFILE).lot
	$(DEL) $(MAINFILE).nlo
	$(DEL) $(MAINFILE).nls
	$(DEL) $(MAINFILE).out
	$(DEL) $(MAINFILE).toc

# Aux-Dateien loeschen (auch von anderen Dateien)
cleanaux:
	$(DEL) *.aux

# Sicherheitskopien loeschen
cleanbackup:
	$(DEL) $(BACKUPFILES)

# PDF-Dokument loeschen
cleanpdf:
	$(DEL) $(MAINFILE).pdf

# Alle temporaren Dateien in einem Durchgang loeschen
cleanall: clean cleanaux cleanbackup

# PDF-Dokument anschauen
showpdf: $(MAINFILE).pdf
	$(VIEWPDF) $(MAINFILE).pdf
