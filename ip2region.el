;;; ip2region.el --- Emacs module of ip2region  -*- lexical-binding: t; -*-

;; Copyright (C) 2017  Chunyang Xu

;; Author: Chunyang Xu <mail@xuchunyang.me>
;; Keywords:

;; This program is free software; you can redistribute it and/or modify
;; it under the terms of the GNU General Public License as published by
;; the Free Software Foundation, either version 3 of the License, or
;; (at your option) any later version.

;; This program is distributed in the hope that it will be useful,
;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;; GNU General Public License for more details.

;; You should have received a copy of the GNU General Public License
;; along with this program.  If not, see <https://www.gnu.org/licenses/>.

;;; Commentary:

;; IP 地址定位.

;;; Code:

(require 'ip2region-module)

(defvar ip2region-db-file (expand-file-name
                           "ip2region/data/ip2region.db"
                           (file-name-directory
                            (or load-file-name buffer-file-name))))

(defvar ip2region--obj nil "ip2region_entry type in C.")

;;;###autoload
(defun ip2region (ip)
  "返回 IP 所在的物理位置."
  (interactive "sIP address: ")
  (unless ip2region--obj
    (setq ip2region--obj
          (ip2region-module-create ip2region-db-file)))
  (unless ip2region--obj
    (error "ip2region-module-create failed"))
  (let ((location (ip2region-module-search ip2region--obj ip)))
    (when (called-interactively-p 'any)
      (message "%S" location))
    location))

;; XXX: Support domain and even URL

(provide 'ip2region)
;;; ip2region.el ends here
