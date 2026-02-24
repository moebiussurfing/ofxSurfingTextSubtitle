#include "ofxSurfingTextSubtitle.h"

#ifdef USE_IM_GUI__SUBTITLES

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::drawImGui() {
	//Fix ugly workaround to avoid dragging box together with ui
	if (ui->isMouseOverGui() && box.isMouseOver()) {
		if (box.isEditing()) box.setToggleEdit();
	} else {
	}

	//--

	drawImGuiWindowMain();

	//--

	//#ifdef USE_PRESETS__SUBTITLES
	//presets.drawImGui(true);
	//#endif

	//--

	if (bGui) {

		drawImGuiWindowParagraph();

		//--

		if (bLoadedFileSubs || bLoadedFileText) {
			if (bGui_List) {
				if (bGui_Paragraph)
					ui->setNextWindowAfterWindowNamed(bGui_Paragraph);
				else
					ui->setNextWindowAfterWindowNamed(bGui);

				drawImGuiWindowList();
			}
		}
	}

	//--

	//ui->AddSpacingSeparated();

	#ifdef USE_PRESETS__SUBTITLES
	if (presets.bGui) {
		if (bGui_List)
			ui->setNextWindowAfterWindowNamed(bGui_List);
		else if (bGui_Paragraph)
			ui->setNextWindowAfterWindowNamed(bGui_Paragraph);
		//else if (bGui) ui->setNextWindowAfterWindowNamed(bGui);
		else
			ui->setNextWindowAfterWindowNamed(bGui);

		presets.drawImGui(true, false, false, false);
	}
	#endif
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::drawImGuiWindowParagraph() {
	if (bGui && bGui_Paragraph) ui->setNextWindowAfterWindowNamed(bGui);

	//if (ui->BeginTree("PARAGRAPH", false, false))
	if (ui->BeginWindow(bGui_Paragraph)) {
		string s;

		ui->Add(bFine, OFX_IM_TOGGLE_ROUNDED_MINI);
		ui->AddSpacing();

		//ui->PushWidth(0.7f);
		SurfingGuiTypes st = (bFine.get() ? OFX_IM_STEPPER : OFX_IM_DEFAULT);
		ui->Add(fSizePrc, st);
		ui->Add(fSpacing, st);
		ui->Add(fLineHeight, st);
		//ui->PopWidth();

		ui->AddSpacing();

		if (bMinimize) // minimized
		{
			ui->AddCombo(fAlign, names_Align);
			ui->AddTooltip("Align");
			ui->AddSpacingSeparated();

			ui->Add(bCenteredV, OFX_IM_TOGGLE_ROUNDED_MINI);
			s = "Y center on container.";
			ui->AddTooltip(s);

			ui->Add(yOffset, st);

			ui->AddSpacingSeparated();
			ui->Add(bResponsive, OFX_IM_TOGGLE_ROUNDED_MINI);
			s = "Use responsive engine.";
			ui->AddTooltip(s);
			ui->AddSpacing();
			if (bResponsive) {
				ui->Add(resizeResponsive, OFX_IM_HSLIDER_SMALL);
				//s = "Rescale";
				ui->AddTooltip(resizeResponsive);
			}
			if (bCenteredV || bResponsive) ui->Add(amountLinesTarget, OFX_IM_STEPPER);

			ui->AddSpacingSeparated();

			ui->Add(bReset, OFX_IM_BUTTON_SMALL);
		} else // maximized
		{
			ui->AddCombo(fAlign, names_Align);
			ui->AddTooltip("Align");

			ui->AddSpacingSeparated();

			if (ui->BeginTree("EXTRA", false, false)) {
				ui->AddSpacing();
				ui->Add(bCenteredV, OFX_IM_TOGGLE_ROUNDED_MINI);
				s = "Y center on container.";
				ui->AddTooltip(s);
				ui->Add(yOffset, st);
				ui->AddSpacingSeparated();
				ui->Add(bResponsive, OFX_IM_TOGGLE_ROUNDED_MINI);
				s = "Use responsive engine.";
				ui->AddTooltip(s);
				if (bResponsive) {
					ui->Indent();
					ui->AddSpacing();
					ui->Add(resizeResponsive, OFX_IM_STEPPER);
					ui->Add(resizeResponsive, OFX_IM_HSLIDER_SMALL_NO_LABELS);
					s = "Rescale";
					ui->AddTooltip(s);
					ui->Unindent();
				}
				if (bCenteredV || bResponsive) {
					ui->AddSpacing();
					ui->Add(amountLinesTarget, OFX_IM_STEPPER);
					s = "Expented amount lines";
					ui->AddTooltip(s);
				}

				ui->AddSpacingSeparated();

				ui->Add(bReset, OFX_IM_BUTTON_SMALL);

				if (ui->AddButton("R1", OFX_IM_BUTTON, 2, true)) {
					fSizePrc = fSizePrc.getMin();
					bCenteredV = true;
					bResponsive = false;
				}
				if (ui->AddButton("R2", OFX_IM_BUTTON, 2)) {
					bCenteredV = false;
					bResponsive = false;
				}

				ui->AddSpacingSeparated();

				if (ui->BeginTree("CONTAINER", false, false)) {
					ui->PushWidth(0.7f);

					//ui->AddLabel("Container");
					static float _x;
					//if (fAlign != 1)
					{
						_x = box.getX();
						if (ImGui::SliderFloat("x", &_x, 0, ofGetWidth())) {
							box.setX(_x);
						}
					}
					//else {
					//	_x = ofGetWidth() - box.getX();
					//	if (ImGui::SliderFloat("x", &_x, 0, ofGetWidth())) {
					//		box.setX(ofGetWidth() - _x);
					//	}
					//}

					static float _y;
					_y = box.getY();
					if (ImGui::SliderFloat("y", &_y, 0, ofGetHeight())) {
						box.setY(_y);
					}

					static float _w;
					_w = box.getWidth();
					if (ImGui::SliderFloat("w", &_w, 0, ofGetWidth())) {
						box.setWidth(_w);
					}

					if (!bCenteredV && !bResponsive) {
						static float _h;
						_h = box.getHeight();
						if (ImGui::SliderFloat("h", &_h, 0, ofGetHeight())) {
							box.setHeight(_h);
						}
					}

					if (ui->AddButton("Reset")) {
						box.reset();
						//box.reset(0.9f);
					}

					ui->PopWidth();

					ui->EndTree(false);
				}

				//--

				if (!bMinimize) {
					ui->AddSpacingSeparated();

					//static bool bDebug3 = false;
					ui->AddToggle("Debug Advanced", bDebug2, OFX_IM_TOGGLE_ROUNDED_MINI);
					s = "Show internal vars from \nresponsive formatting engine.";
					ui->AddTooltip(s);
					ui->AddSpacing();
					ui->AddSpacing();

					if (bDebug2) {
						string s;
						ui->Indent();

						float ho = this->oneLineHeight;
						//float ho = this->getOneLineHeight();
						s = "H Line: " + ofToString(ho);
						ui->AddLabel(s);
						float hb = box.getHeight();
						s = "H Box: " + ofToString(hb);
						ui->AddLabel(s);
						s = "Expected Lines: " + ofToString(amountLinesTarget);
						ui->AddLabel(s);
						s = "Drawn: " + ofToString(amountLinesDrawn);
						ui->AddLabel(s);
						s = "Diff: " + ofToString(diff);
						ui->AddLabel(s);
						s = "Used Engine: " + sEngine;
						ui->AddLabel(s);

						ui->Unindent();
					}
				}

				ui->EndTree(false);
			}
		}

		//ui->EndTree();
		ui->EndWindow();
	}
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::drawImGuiWindowMain() {
	/*
	//IMGUI_SUGAR__WINDOWS_CONSTRAINTSW;
	float w = 290;
	//float w = 250;
	ImVec2 size_max = ImVec2(w, -1);
	ImVec2 size_min = ImVec2(w, -1);
	ImGui::SetNextWindowSizeConstraints(size_min, size_max);
	*/

	if (ui->BeginWindow(bGui)) //main
	{
		drawImGuiWidgets();

		ui->EndWindow();
	}
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::drawImGuiWidgets() {
	// time label
	string sfile = "";
	string sdialog = "";
	string stime = "";
	string s;

	//--

	if (bLoadedFileSubs) {
		// filename
		if (bModeTextBlocks)
			sfile = name_Text;
		else
			sfile = name_Srt;

		// Elapsed time

		if (indexModes == 0) // EXTERNAL
		{
			if (bPlayExternal) {
				auto t = positionExternal * (float)tEndSubsFilm;
	#ifdef USING_OFX_TIME_CODE
				stime = timecode.timecodeForMillis(t);
	#else
				stime = t / 1000.f + "''";
	#endif
			}
		} else if (indexModes == 1) // STANDALONE
		{
			if (bPlayStandalone) {
				uint64_t t = ofGetElapsedTimeMillis() - tPlay;

	#ifdef USING_OFX_TIME_CODE
				stime = timecode.timecodeForMillis(t);
	#else
				stime += t / 1000.f;
	#endif
			}
		} else if (indexModes == 2) // FORCED
		{
			if (bPlayForced) {
				if (bModeTextBlocks) {
					// not included times. just a text file.
				} else
					stime = sub[currentDialog]->getStartTimeString();
			}
		} else if (indexModes == 3) // MANUAL
		{
			if (bPlayManual) {
			}
		}

		if (!bPlayStandalone && !bPlayForced && !bPlayExternal && !bPlayManual) {
			if (bModeTextBlocks)
				stime = "";
			else
				stime = sub[currentDialog]->getStartTimeString();
			//stime = timecode.timecodeForMillis(0);
		}

		// index
		sdialog = ofToString(currentDialog) + "/" + ofToString(sub.size() - 1);
	}

	////workflow
	//if (!bLoadedFileSubs && !bLoadedFileText && indexModes != 3)
	//	indexModes = 3;

	//----

	ui->Add(bMinimize, OFX_IM_TOGGLE_ROUNDED_SMALL);
	if (!bMinimize) {
		ui->Add(bKeys, OFX_IM_TOGGLE_ROUNDED_MINI);
		ui->Add(bDebug, OFX_IM_TOGGLE_ROUNDED_MINI);
	}

	//--

	// Manual
	//workflow
	if (indexModes != 3) {
		if (!bMinimize) // maximized
		{
			ui->AddSpacingSeparated();

			std::string n = "FILE";
			if (ui->BeginTree(n)) {
				ui->Add(bOpenSrt, OFX_IM_BUTTON_SMALL);
				s = "Open a Srt file";
				ui->AddTooltip(s);
				if (name_Srt != "") {
					ui->AddLabel(name_Srt + ".srt");
					ui->AddTooltip(path_Srt);
				}
				ui->AddSpacing();

				ui->Add(bOpenText, OFX_IM_BUTTON_SMALL);
				s = "Open a Text file";
				ui->AddTooltip(s);
				if (name_Text != "") {
					ui->AddLabel(name_Text + ".txt");
					ui->AddTooltip(path_Text);
				}
				ui->EndTree(false);
			}
		}

		//--

		if (bLoadedFileSubs || bLoadedFileText) {
			if (!bModeTextBlocks) {
				ui->AddSpacingSeparated();

				if (!bMinimize) {
					ui->AddLabelBig(sfile);
					s = path_Srt;
					ui->AddTooltip(s);
				}
				ui->AddLabelBig(stime);

				ui->AddSpacing();

				ui->Add(progressPlayFilm, OFX_IM_PROGRESS_BAR);
				s = "Total Duration";
				ui->AddTooltip(s);

				ui->Add(progressPlaySlide, OFX_IM_PROGRESS_BAR_NO_TEXT);
				s = "Dialog Duration";
				ui->AddTooltip(s);
			}
		} else {
			ui->BeginBlinkText();
			ui->AddLabelBig("FILE NOT LOADED!");
			ui->EndBlinkText();
			ui->AddSpacing();
			ui->Add(bOpenSrt, OFX_IM_BUTTON_SMALL);
			s = "Open a Srt file";
			ui->Add(bOpenText, OFX_IM_BUTTON_SMALL);
			s = "Open a Text file";
			ui->AddTooltip(s);
		}
	}

	//----

	if (bLoadedFileSubs || bLoadedFileText || (indexModes == 3)) {
		ui->AddSpacingSeparated();

		//if (bMinimize)
		{
			ui->Add(bDraw, OFX_IM_TOGGLE_BIG_BORDER_BLINK);
			ui->Add(bLive, OFX_IM_TOGGLE_MEDIUM_BORDER_BLINK);
			s = "Live Mode hides some stuff";
			if ((!bLive && !bEdit) || bEdit) {
				ui->Add(bEdit, OFX_IM_TOGGLE);
				s = "Edit container";
				ui->AddTooltip(s);
			}

			ui->AddSpacingSeparated();
		}

		if (ui->BeginTree("WINDOWS", false, false)) {
			ui->AddSpacing();

			ui->Add(bGui_Paragraph, OFX_IM_TOGGLE_ROUNDED);

			// not manual
			//if (indexModes != 3)
			{
				ui->Add(bGui_List, OFX_IM_TOGGLE_ROUNDED);
			}

	#ifdef USE_PRESETS__SUBTITLES
			ui->AddSpacing();
			ui->Add(presets.bGui, OFX_IM_TOGGLE_ROUNDED);
	#endif

			//if (!bLive)
			//{
			//	//ui->AddSpacingSeparated();
			//	ui->AddSpacing();
			//}

			ui->EndTree(false);
		} //main

		//--

		ui->AddSpacingSeparated();

		// maximized
		//if (!bMinimize)
		{
			if (ui->BeginTree("TRANSPORT", true, false)) {
				ui->AddSpacing();

				//if (!bModeNoSrt)
				{
					ui->AddCombo(indexModes, names_Modes);
					string s = "Link Mode ";
					switch (indexModes) {
					case 0:
						s += "EXTERNAL\nLocal time linked \nto parent player.";
						break;
					case 1:
						s += "STANDALONE\nTime linked to \n.srt file times.";
						break;
					case 2:
						s += "FORCED\nLocal time linked \nto his timer.";
						break;
					case 3:
						s += "MANUAL\nIgnore Srt files \nand load any text.\nCalled by code.";
						break;
					}
					ui->AddTooltip(s);
				}

				if (indexModes == 0) // EXTERNAL
				{
					ui->Add(positionExternal, OFX_IM_HSLIDER_MINI);
					s = "Local time linked to parent player.";
					ui->AddTooltip(s);
					ui->Add(durationPlayForced, OFX_IM_HSLIDER_MINI);
					ui->AddTooltip(ofToString(durationPlayForced.get()) + "\nms");
				} else if (indexModes == 1) // STANDALONE
				{
					ui->Add(bPlayStandalone, OFX_IM_TOGGLE_BIG_BORDER_BLINK);
					s = "Time linked to .srt file times.";
					ui->AddTooltip(s);
					ui->Add(bStop, OFX_IM_BUTTON_SMALL);
					ui->Add(currentDialog, OFX_IM_HSLIDER_MINI_NO_NAME);
					s = "Dialog \n#" + ofToString(currentDialog.get());
				} else if (indexModes == 2) // FORCED
				{
					ui->Add(bPlayForced, OFX_IM_TOGGLE_BIG_BORDER_BLINK);
					s = "Local time linked to his timer.";
					ui->AddTooltip(s);
					ui->Add(bStop, OFX_IM_BUTTON_SMALL);
					ui->Add(durationPlayForced, OFX_IM_HSLIDER_MINI);
					//ui->Add(durationPlayForced);
					//ui->Add(durationPlayForced, OFX_IM_HSLIDER);
					ui->AddTooltip(ofToString(durationPlayForced.get()) + "\nms");
					ui->Add(currentDialog, OFX_IM_HSLIDER_MINI_NO_NAME);
					s = "Dialog \n#" + ofToString(currentDialog.get());
					ui->AddTooltip(s);
					//ui->AddTooltip(currentDialog);

					ui->PushButtonRepeat();
					ui->Add(bPrev, OFX_IM_BUTTON_BIG, 2, true);
					ui->Add(bNext, OFX_IM_BUTTON_BIG, 2);
					ui->PopButtonRepeat();
				} else if (indexModes == 3) // MANUAL
				{
					ui->Add(bPlayManual, OFX_IM_TOGGLE_BIG_BORDER_BLINK);
					ui->Add(durationPlayForced, OFX_IM_HSLIDER_MINI);
					ui->AddTooltip(ofToString(durationPlayForced.get()) + "\nms");
				}

				//if (!bPlayExternal || indexModes != 2) ui->Add(bStop, OFX_IM_BUTTON_SMALL);

				ui->EndTree(false);
			}

			ui->AddSpacingSeparated();
		}

		//--

		if (ui->BeginTree("FADES", false, false)) {
			string s;

			ui->Add(progressPlaySlide, OFX_IM_PROGRESS_BAR);
			s = "Slide progress";
			ui->AddTooltip(s);
			ui->AddSpacing();

			if (bMinimize) {
				ui->Add(bAnimatedFades, OFX_IM_TOGGLE_SMALL);
				if (bAnimatedFades) {
					// in
					ui->Add(bAnimatedIn, OFX_IM_TOGGLE_SMALL, 2, true);
					s = "Fade In";
					ui->AddTooltip(s);

					// out
					ui->Add(bAnimatedOut, OFX_IM_TOGGLE_SMALL, 2);
					s = "Fade Out";
					ui->AddTooltip(s);

					if (bAnimatedIn) {
						ui->Add(progressIn, OFX_IM_PROGRESS_BAR_NO_TEXT);
						s = ofToString(durationIn.get(), 0);
						s += " ms";
						ui->AddTooltip(s);
					}
					if (bAnimatedOut) {
						ui->Add(progressOut, OFX_IM_PROGRESS_BAR_NO_TEXT);
						s = ofToString(durationOut.get(), 0);
						s += " ms";
						ui->AddTooltip(s);
					}
				}
			} else {
				ui->Add(bAnimatedFades, OFX_IM_TOGGLE_SMALL);
				if (bAnimatedFades) {
					// in
					ui->Add(bAnimatedIn, OFX_IM_TOGGLE_SMALL, 2, true);
					s = "Fade In";
					ui->AddTooltip(s);
					if (bAnimatedIn) {
						ui->Add(progressIn, OFX_IM_PROGRESS_BAR_NO_TEXT);
						ui->Add(durationIn, OFX_IM_HSLIDER_MINI_NO_LABELS);
						ui->AddTooltip(durationIn);
					}
					ui->AddSpacing();

					// out
					ui->Add(bAnimatedOut, OFX_IM_TOGGLE_SMALL, 2, true);
					s = "Fade Out";
					ui->AddTooltip(s);
					if (bAnimatedOut) {
						ui->Add(progressOut, OFX_IM_PROGRESS_BAR_NO_TEXT);
						ui->Add(durationOut, OFX_IM_HSLIDER_MINI_NO_LABELS);
						ui->AddTooltip(durationOut);
					}
					ui->AddSpacing();

					ui->Add(bResetFades, OFX_IM_BUTTON_SMALL);
				}
			}

			ui->EndTree(false);
		}

		ui->AddSpacingSeparated();

		if (ui->BeginTree("STYLE", false, false)) {
			if (!bMinimize) {
				ofxImGuiSurfing::AddToggleNamed(bCapitalize, "CAPITALIZE", "Capitalize");

				ui->AddSpacing();

				if (ui->BeginTree("COLORS", false, false)) {
					ui->PushWidth(0.7);
					static bool bShowBg = false;
					static float _alpha;
					_alpha = colorTextFloat.get().a;
					if (ImGui::SliderFloat("Alpha", &_alpha, 0, 1)) {
						ofFloatColor c = colorTextFloat.get();
						colorTextFloat.set(ofFloatColor(c.r, c.g, c.b, _alpha));

						doRefreshDraw();
					}
					ui->PopWidth();

					ui->Add(colorTextFloat, OFX_IM_COLOR_BOX_FULL_WIDTH_NO_ALPHA);
					ui->Add(colorBgFloat, OFX_IM_COLOR_BOX_FULL_WIDTH_NO_ALPHA);

					if (ui->AddButton("Swap")) {
						ofFloatColor c = colorTextFloat;
						colorTextFloat = colorBgFloat;
						colorBgFloat = c;

						doRefreshDraw();
					}

					ui->EndTree(false);
				}
			} else {
				ui->Add(colorTextFloat, OFX_IM_COLOR_BOX_FULL_WIDTH_NO_ALPHA);
				ui->Add(colorBgFloat, OFX_IM_COLOR_BOX_FULL_WIDTH_NO_ALPHA);
			}

			ui->EndTree(false);
		}

		//--

		if (!bLive) {
			if (!bMinimize) {
				ui->AddSpacingSeparated();

				ui->Indent();
				{
					ui->Add(bExtra, OFX_IM_TOGGLE_ROUNDED);
					if (bExtra) {
						ui->Indent();
						{
							ui->AddSpacing();
							ui->Add(bDebug, OFX_IM_TOGGLE_ROUNDED_MINI);
							ui->Add(ui->bThemeUIAlt, OFX_IM_TOGGLE_ROUNDED_MINI);
							ui->Add(bUseFbo, OFX_IM_TOGGLE_ROUNDED_MINI);
							s = "Debug HUD";
							ui->AddLabel(s);
							ui->Add(bTheme, OFX_IM_TOGGLE_ROUNDED_MINI);
							ui->Add(bTop, OFX_IM_TOGGLE_ROUNDED_MINI);
							ui->Add(bLeft, OFX_IM_TOGGLE_ROUNDED_MINI);

	#ifndef USE_IM_GUI__SUBTITLES
							if (bEdit && bGui_InternalAllowed) ui->Add(bGui_Internal, OFX_IM_TOGGLE_BUTTON_ROUNDED_MINI);
	#endif

	#ifdef USE_WIDGET__SUBTITLES
							ui->Add(bDrawWidgetInfo, OFX_IM_TOGGLE_BUTTON_ROUNDED_MINI);
	#endif
							if (bDebug) {
								ui->AddSpacing();
								ui->Add(ui->bDebugDebuggerImGui, OFX_IM_TOGGLE_ROUNDED_MINI);
								if (bModeTextBlocks)
									s = "MODE Text file";
								else
									s = "MODE Srt file";
								ui->AddLabel(s);
								s = "MODE #" + ofToString(indexModes.get()) + string(" ") + indexModes_Name.get();
								ui->AddLabel(s);
							}
						}
						ui->Unindent();
					}
					ui->AddSpacing();
				}
				ui->Unindent();
			}
		}

		ui->AddSpacingSeparated();
		ui->AddSpacing();
		ui->Add(bResetAll, OFX_IM_BUTTON_SMALL);

		//--

		//		//ui->AddSpacingSeparated();
		//
		//#ifdef USE_PRESETS__SUBTITLES
		//		if (presets.bGui)
		//		{
		//			//ui->AddSpacingSeparated();
		//			ImGui::Spacing();
		//			ImGui::Separator();//fix
		//			ImGui::Spacing();
		//
		//			presets.drawImGui(false, false, true, false);
		//		}
		//#endif
	}
}

//--------------------------------------------------------------
void ofxSurfingTextSubtitle::drawImGuiWindowList() {
	if (bGui_List) {
		//IMGUI_SUGAR__WINDOWS_CONSTRAINTSW_MEDIUM;
		/*
		float w = 200;
		ImVec2 size_max = ImVec2(-1, -1);
		ImVec2 size_min = ImVec2(w, -1);
		ImGui::SetNextWindowSizeConstraints(size_min, size_max);
		*/
		/*
		ImVec2 width(300, 900);
		ImVec2 height(300, 900);
		ofxImGuiSurfing::SetWindowContraints(ImVec2(width.x, height.x), ImVec2(width.y, height.y));
		*/

		ImVec2 sz = ImVec2(300, 600);
		ImGui::SetNextWindowSize(sz, ImGuiCond_FirstUseEver);

		//--

		//TODO:
		// Alternate modes:
		// Pick text data from srt file or from text block files:

		if (bModeTextBlocks) {
			dataTextPtr = &dataTextBlocks;
		} else {
			dataTextPtr = &dataTextSubs;
		}

		//--

		if (ui->BeginWindow(bGui_List, ImGuiWindowFlags_None)) {
			if (!bMinimize) {
				if (bModeTextBlocks) {
					ui->AddLabelBig(name_Text + ".txt");
					ui->AddTooltip(path_Text);
				} else {
					ui->AddLabelBig(name_Srt + ".srt");
					ui->AddTooltip(path_Srt);
				}
				ui->AddSpacing();

				ui->Add(progressPlayFilm, OFX_IM_PROGRESS_BAR);
				ui->AddSpacing();

				int track_item = currentDialog;
				ui->Add(bAutoScroll, OFX_IM_TOGGLE_ROUNDED_MINI);

				ui->AddSpacingBig();
				//ui->AddSpacingBigSeparated();
			}

			ImVec2 sz(0, 0);
			bool border = true;
			ImGuiWindowFlags extra_flags = ImGuiWindowFlags_None;
			if (bAutoScroll) extra_flags += ImGuiWindowFlags_NoScrollbar;

			ImGui::BeginChild("_", sz, border, extra_flags);
			{
				// iterate all srt dialogs
				for (int n = 0; n < dataTextPtr->size(); n++) {
					/*if (currentDialog == n)*/ ui->AddSeparated();

					float w = ui->getWidgetsWidth(1);
					float h = 1.5 * ui->getWidgetsHeightUnit();

					float w1 = 40;
					float w2 = w - w1;

					ImGui::Columns(2, "t", false);
					ImGui::SetColumnWidth(0, w1);
					ImGui::SetColumnWidth(1, w2);

					string s = ofToString(n);
					//ui->AddLabel(s);

					bool bDoUpdate = false;

					if (currentDialog == n) ui->BeginBorderFrame();

					if (ImGui::Button(s.c_str(), ImVec2(ImGui::GetContentRegionAvail().x, h))) {
						//workflow
						if (bPlayExternal) bPlayForced = true;

						//currentDialog = n;
						bDoUpdate = true;
					}

					if (currentDialog == n) ui->EndBorderFrame();

					if (bDoUpdate) currentDialog = n;

					//if (!bMinimize) {
					//	ui->AddLabel(sub[n]->getStartTimeString());
					//	ui->AddLabel(sub[n]->getEndTimeString());
					//}

					ImGui::NextColumn();

					if (currentDialog == n) {
						//ImGui::ButtonEx(dataTextSubs[n].c_str(), ImVec2(w, 40));
						//ui->AddLabelBig(dataTextSubs[n]);//bigger if selected
						ui->AddLabelBig((*dataTextPtr)[n]); //bigger if selected

						if (bAutoScroll) {
							ImGui::SetScrollHereY(0.0f);
							//ImGui::SetScrollHereY(0.5f);
							//if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
						}

						//--
					}
					//else ui->AddLabelBig(dataTextSubs[n]);
					else
						ui->AddLabelBig((*dataTextPtr)[n]);

					if (!bMinimize || currentDialog == n) {
						if (!bModeTextBlocks) {
							string s;

							s = sub[n]->getStartTimeString();
							s += "   ";
							s += sub[n]->getEndTimeString();

							ui->AddSpacing();

							ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetColumnWidth() - ImGui::CalcTextSize(s.c_str()).x - ImGui::GetScrollX() - 2 * ImGui::GetStyle().ItemSpacing.x);

							ui->AddLabel(s);
						}
					}

					ImGui::Columns(1);

					//*if (currentDialog == n) ui->AddSeparated();
				}
			}
			ImGui::EndChild();

			ui->EndWindow();
		}
	}
}

#endif
