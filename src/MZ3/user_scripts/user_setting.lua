mz3.logger_debug('user_setting.lua start');

----------------------------------------------------------------------
-- このファイルはMZ3/4の詳細な設定を変更するためのものです。
-- 
-- 設定画面を用意するほどでもない、マニアックな設定を対象としています。
----------------------------------------------------------------------

------------------------------
-- クロスポスト関連
------------------------------

-- Wassr 等での発言後に Twitter でも発言するか？
-- (有効にするためには次の行の先頭の--を削除してください)
--use_cross_post_to_twitter = true;

-- Twitter 等での発言後に Wassr でも発言するか？
-- (有効にするためには次の行の先頭の--を削除してください)
--use_cross_post_to_wassr = true;

-- Twitter 等での発言後に mixiエコー でも発言するか？
-- (mixiエコーへの投稿は事前にエコー一覧を取得する必要があります)
--use_cross_post_to_echo = true;


------------------------------
-- Twitter の詳細設定
------------------------------

-- Twitter の返信時に「@xxx」ではなく「.@xxx」とする
-- こうすることであなたのフォロワーにその発言が届きます。
-- 「@xxx こんにちは」のような発言はあなたのフォロワーのTLには表示されません。
--use_twitter_reply_with_dot = true;


------------------------------
-- 2ch の詳細設定
------------------------------

-- 2ch スレの最大取得件数
t2ch_thread_view_limit = 50;

mz3.logger_debug('user_setting.lua end');
