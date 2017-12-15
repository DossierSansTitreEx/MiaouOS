let s:cpo_save=&cpo
set cpo&vim
inoremap <silent> <C-Tab> =UltiSnips#ListSnippets()
inoremap <Nul> 
inoremap <C-Space> 
noremap  
snoremap <silent>  c
xnoremap <silent> 	 :call UltiSnips#SaveLastVisualSelection()gvs
snoremap <silent> 	 :call UltiSnips#ExpandSnippet()
nnoremap <silent> 	 :NERDTreeToggle
onoremap <silent> 	 :NERDTreeToggle
xnoremap <silent>  :call multiple_cursors#new("v", 0)
nnoremap <silent>  :call multiple_cursors#new("n", 1)
map  <Plug>(ctrlp)
snoremap  "_c
nnoremap   za
nmap ,ihn :IHN
nmap ,is :IHS:A
nmap ,ih :IHS
nmap ,ru :RuboCop
nmap ,hp <Plug>GitGutterPreviewHunk
nmap ,hr <Plug>GitGutterUndoHunk:echomsg ',hr is deprecated. Use ,hu'
nmap ,hu <Plug>GitGutterUndoHunk
nmap ,hs <Plug>GitGutterStageHunk
nnoremap ,a :Ack! 
noremap <silent> ,. :edit .
noremap <silent> ,  :nohlsearch
noremap <silent> < :tabp
noremap <silent> > :tabn
vmap [% [%m'gv``
nmap [c <Plug>GitGutterPrevHunk
vmap ]% ]%m'gv``
nmap ]c <Plug>GitGutterNextHunk
vmap a% [%v]%
xmap ac <Plug>GitGutterTextObjectOuterVisual
omap ac <Plug>GitGutterTextObjectOuterPending
vmap gx <Plug>NetrwBrowseXVis
nmap gx <Plug>NetrwBrowseX
xmap ic <Plug>GitGutterTextObjectInnerVisual
omap ic <Plug>GitGutterTextObjectInnerPending
vnoremap <silent> <Plug>NetrwBrowseXVis :call netrw#BrowseXVis()
nnoremap <silent> <Plug>NetrwBrowseX :call netrw#BrowseX(netrw#GX(),netrw#CheckIfRemote(netrw#GX()))
nnoremap <SNR>41_: :=v:count ? v:count : ''
snoremap <silent> <Del> c
snoremap <silent> <BS> c
snoremap <silent> <C-Tab> :call UltiSnips#ListSnippets()
nnoremap <silent> <Plug>(ctrlp) :CtrlP
nnoremap <silent> <Plug>GitGutterPreviewHunk :GitGutterPreviewHunk
nnoremap <silent> <Plug>GitGutterUndoHunk :GitGutterUndoHunk
nnoremap <silent> <Plug>GitGutterStageHunk :GitGutterStageHunk
nnoremap <silent> <expr> <Plug>GitGutterPrevHunk &diff ? '[c' : ":\execute v:count1 . 'GitGutterPrevHunk'\"
nnoremap <silent> <expr> <Plug>GitGutterNextHunk &diff ? ']c' : ":\execute v:count1 . 'GitGutterNextHunk'\"
xnoremap <silent> <Plug>GitGutterTextObjectOuterVisual :call gitgutter#hunk#text_object(0)
xnoremap <silent> <Plug>GitGutterTextObjectInnerVisual :call gitgutter#hunk#text_object(1)
onoremap <silent> <Plug>GitGutterTextObjectOuterPending :call gitgutter#hunk#text_object(0)
onoremap <silent> <Plug>GitGutterTextObjectInnerPending :call gitgutter#hunk#text_object(1)
nnoremap <silent> <F4> :TagbarToggle
noremap <silent> <F3> :ToggleNumber
noremap <F5> :Fixup
inoremap <silent> 	 =UltiSnips#ExpandSnippet()
imap ,ihn :IHN
imap ,is :IHS:A
imap ,ih :IHS
let &cpo=s:cpo_save
unlet s:cpo_save
set backup
set backupdir=~/.vim-tmp//,~/.tmp//,~/tmp//,/var/tmp//,/tmp//
set cinoptions=g0,t0,N-s,+0,i0
set completeopt=longest,menuone
set directory=~/.vim-tmp//,~/.tmp//,~/tmp//,/var/tmp//,/tmp//
set expandtab
set fileencodings=utf-8
set helplang=en
set lazyredraw
set mouse=a
set pastetoggle=<F2>
set runtimepath=~/.config/nvim,~/.vim/plugged/vim-gitgutter/,~/.vim/plugged/ctrlp.vim/,~/.vim/plugged/editorconfig-vim/,~/.vim/plugged/vim-elixir/,~/.vim/plugged/bufexplorer/,~/.vim/plugged/vim-syntax-extra/,~/.vim/plugged/tagbar/,~/.vim/plugged/ack.vim/,~/.vim/plugged/vim-jsx/,~/.vim/plugged/vim-rubocop/,~/.vim/plugged/rootignore/,~/.vim/plugged/html5.vim/,~/.vim/plugged/vim-javascript/,~/.vim/plugged/vim-cmake-syntax/,~/.vim/plugged/clang_complete/,~/.vim/plugged/nerdtree/,~/.vim/plugged/neocomplete.vim/,~/.vim/plugged/UltiSnips/,~/.vim/plugged/vim-tags/,~/.vim/plugged/vim-multiple-cursors/,~/.vim/plugged/vim-bundler/,~/.vim/plugged/vim-fugitive/,~/.vim/plugged/vim-rails/,~/.vim/plugged/vim-cmake/,~/.vim/plugged/vim-airline/,~/.vim/plugged/a.vim/,~/.vim/plugged/xoria256.vim/,/etc/xdg/nvim,~/.local/share/nvim/site,/usr/local/share/nvim/site,/usr/share/nvim/site,/opt/local/share/nvim/runtime,/usr/share/nvim/site/after,/usr/local/share/nvim/site/after,~/.local/share/nvim/site/after,/etc/xdg/nvim/after,~/.vim/plugged/vim-synt
set shiftwidth=4
set showmatch
set noshowmode
set softtabstop=4
set tabstop=4
set tags=./tags;,tags,.git/tags
set updatetime=250
set wildignore=tags,.DS_Store,build
set window=61
" vim: set ft=vim :
